"""Auralix C Library - AlxPca9539a on the host.

A 16-pin IO expander: two 8-bit ports, each with an input, an output, a polarity inversion and a
configuration (direction) register. The driver keeps a shadow of the registers and talks to the
part only in Init and Handle: pin writes change the output shadow, Handle sends it and reads the
inputs back, and pin reads answer from what the last Handle read.

One pin is special by default: pin 6 of port 0 is emulated open-drain, driven by switching its
direction instead of its level.

Proofs (ALX-1564, A92): P608-P614.
"""

import pytest

IN0, IN1, OUT0, OUT1, POL0, POL1, CFG0, CFG1 = range(8)


@pytest.fixture
def expander(pca9539a_lib):
    """One expander, initialised over a ready bus."""
    obj = pca9539a_lib.new()
    assert pca9539a_lib.call(obj, "Init") == pca9539a_lib.OK
    return pca9539a_lib, obj


def test_ALX1564_P608_init_makes_every_pin_an_input_without_inversion(pca9539a_lib):
    """Both ports' directions all ones (inputs) and their polarity all zeros, each written once.

    The outputs are not written at Init - the driver leaves them to the application's first
    Handle, so a product decides its outputs before any pin becomes one.
    """
    obj = pca9539a_lib.new()

    assert pca9539a_lib.call(obj, "Init") == pca9539a_lib.OK

    assert [pca9539a_lib.last_write(r, 1) for r in (POL0, POL1, CFG0, CFG1)] == [b"\x00", b"\x00", b"\xff", b"\xff"]
    assert [pca9539a_lib.write_count(r) for r in (POL0, POL1, CFG0, CFG1)] == [1, 1, 1, 1]
    assert [pca9539a_lib.write_count(r) for r in (OUT0, OUT1)] == [0, 0]


def test_ALX1564_P609_handle_reads_both_inputs_and_writes_both_outputs(expander):
    """One Handle is one read of each input port and one write of each output port."""
    lib, obj = expander
    lib.write(obj, 0, 3, True)
    lib.write(obj, 1, 7, True)

    assert lib.call(obj, "Handle") == lib.OK

    assert (lib.read_count(IN0), lib.read_count(IN1)) == (1, 1)
    assert (lib.last_write(OUT0, 1), lib.last_write(OUT1, 1)) == (b"\x08", b"\x80")


def test_ALX1564_P610_a_pin_reads_its_input_bit_as_of_the_last_handle(expander):
    """A read answers from the shadow, so a change on the part shows only after the next Handle."""
    lib, obj = expander
    lib.set_register(IN0, bytes([0b0010_0000]))
    lib.set_register(IN1, bytes([0b0000_0001]))
    lib.call(obj, "Handle")

    assert [lib.read(obj, 0, p) for p in range(8)] == [False] * 5 + [True] + [False] * 2
    assert [lib.read(obj, 1, p) for p in range(8)] == [True] + [False] * 7
    assert (lib.read_port(obj, 0), lib.read_port(obj, 1)) == (0b0010_0000, 0b0000_0001)

    lib.set_register(IN0, bytes([0x00]))
    assert lib.read(obj, 0, 5) is True, "a read went to the part instead of the shadow"
    lib.call(obj, "Handle")
    assert lib.read(obj, 0, 5) is False


def test_ALX1564_P611_set_reset_toggle_and_write_change_only_their_own_bit(expander):
    """Every pin operation is one bit of one port's output shadow, sent by the next Handle."""
    lib, obj = expander
    lib.pin_op(obj, "Set", 0, 0)
    lib.pin_op(obj, "Set", 0, 7)
    lib.pin_op(obj, "Reset", 0, 0)
    lib.pin_op(obj, "Toggle", 1, 4)
    lib.pin_op(obj, "Toggle", 1, 2)
    lib.pin_op(obj, "Toggle", 1, 2)
    lib.write(obj, 1, 0, True)
    assert lib.write_count(OUT0) == 0, "a pin operation reached the part before Handle"

    lib.call(obj, "Handle")

    assert lib.last_write(OUT0, 1) == bytes([0b1000_0000])
    assert lib.last_write(OUT1, 1) == bytes([0b0001_0001])


@pytest.mark.parametrize(("level", "direction"), [(True, 0xFF), (False, 0xBF)])
def test_ALX1564_P612_the_open_drain_pin_is_driven_by_its_direction(expander, level, direction):
    """CHARACTERIZATION: pin 6 of port 0 is open-drain by the driver's own default.

    The part has push-pull outputs only, so the driver emulates open-drain: a high is the pin made
    an input (released, pulled up on the board), a low is the pin made an output driving its
    latch's low. Every Handle rewrites port 0's direction for it; port 1, with no open-drain pin,
    keeps the direction Init wrote. The pin is a constant in the library, the same for every
    product.
    """
    lib, obj = expander
    lib.write(obj, 0, 6, level)

    lib.call(obj, "Handle")

    assert lib.last_write(CFG0, 1) == bytes([direction])
    assert lib.write_count(CFG0) == 2
    assert lib.write_count(CFG1) == 1, "port 1's direction was rewritten without an open-drain pin"


def test_ALX1564_P613_the_interrupt_handler_reads_the_inputs_and_writes_nothing(expander):
    """irqHandle is half a Handle: the two input ports, and no output reaches the part."""
    lib, obj = expander
    lib.write(obj, 0, 1, True)
    lib.set_register(IN1, bytes([0x80]))

    assert lib.call(obj, "irqHandle") == lib.OK

    assert (lib.read_count(IN0), lib.read_count(IN1)) == (1, 1)
    assert lib.read(obj, 1, 7) is True
    assert (lib.write_count(OUT0), lib.write_count(OUT1)) == (0, 0)


def test_ALX1564_P614_init_fails_when_the_part_does_not_answer(pca9539a_lib):
    """No acknowledge: Init reports the error and configures nothing."""
    obj = pca9539a_lib.new()
    pca9539a_lib.slave_ready(False)

    assert pca9539a_lib.call(obj, "Init") == pca9539a_lib.ERR

    assert pca9539a_lib.write_count(CFG0) == 0


# =====================================================================
# P773-P779 - the bus steps, a shared bus, the open-drain of port 1, the pins outside the ports, the weak hook (A97)
# =====================================================================


@pytest.mark.xfail(strict=True, reason=(
    "DEFECT: AlxPca9539a_Init calls AlxPca9539a_InitPeriph and drops its status (alxPca9539a.c, Init), so a bus "
    "that would not initialise is reported as Alx_Ok by Init, with isInitPeriph false - and the next Handle "
    "trips the assertion on it. The fix is to return InitPeriph's status as the other drivers do"))
def test_ALX1564_P773_init_reports_a_bus_that_will_not_initialise(pca9539a_lib):
    obj = pca9539a_lib.new()
    pca9539a_lib.fail_at(1)

    assert pca9539a_lib.call(obj, "Init") == pca9539a_lib.ERR


@pytest.mark.parametrize("nth", [3, 4, 5, 6])
def test_ALX1564_P773_init_stops_at_the_configuration_write_that_fails(pca9539a_lib, nth):
    """The bus's Init, the acknowledge, then the four configuration writes: whichever fails ends Init."""
    obj = pca9539a_lib.new()
    pca9539a_lib.fail_at(nth)

    assert pca9539a_lib.call(obj, "Init") == pca9539a_lib.ERR
    assert sum(pca9539a_lib.write_count(r) for r in (POL0, POL1, CFG0, CFG1)) == nth - 2


def test_ALX1564_P774_a_second_expander_on_an_initialised_bus_does_not_initialise_it_again(pca9539a_lib, expander):
    """Init sees the bus already initialised and only marks its own periph flag: one bus Init for two parts."""
    lib, first = expander
    second = lib.new_on_bus_of(first)
    calls = lib.bus_calls()

    assert lib.call(second, "Init") == lib.OK

    assert lib.bus_calls() == calls + 5, "the acknowledge and the four writes, no bus Init"
    assert lib.call(second, "Handle") == lib.OK, "and it is usable"


def test_ALX1564_P775_deinit_releases_the_bus_and_deinit_periph_reports_one_that_will_not(pca9539a_lib, expander):
    lib, obj = expander
    lib.fail_at(1)
    assert lib.call(obj, "DeInitPeriph") == lib.ERR
    assert lib.call(obj, "DeInitPeriph") == lib.OK
    assert lib.call(obj, "InitPeriph") == lib.OK

    assert lib.call(obj, "DeInit") == lib.OK
    assert lib.call(obj, "Init") == lib.OK, "and the expander comes back"


@pytest.mark.parametrize("nth", [1, 2, 3, 4, 5])
def test_ALX1564_P776_handle_stops_at_the_transfer_that_fails(pca9539a_lib, expander, nth):
    """Two input reads, the open-drain direction write, two output writes: the ones before the failure are made."""
    lib, obj = expander
    before = (lib.read_count(IN0), lib.read_count(IN1), lib.write_count(CFG0), lib.write_count(OUT0), lib.write_count(OUT1))
    lib.fail_at(nth)

    assert lib.call(obj, "Handle") == lib.ERR

    after = (lib.read_count(IN0), lib.read_count(IN1), lib.write_count(CFG0), lib.write_count(OUT0), lib.write_count(OUT1))
    assert [a - b for a, b in zip(after, before)] == [1] * nth + [0] * (5 - nth)


def test_ALX1564_P776_an_open_drain_pin_on_port_1_is_driven_by_its_direction_too(pca9539a_lib, expander):
    """P612 for the other port: with the open-drain mask on port 1 alone, its direction register is written."""
    lib, obj = expander
    lib.open_drain(obj, 0x00, 0x40)
    lib.write(obj, 1, 6, True)

    assert lib.call(obj, "Handle") == lib.OK
    assert lib.last_write(CFG1, 1) == b"\xff", "an open-drain pin driven high is an input"
    assert lib.write_count(CFG0) == 1, "port 0's direction was written by Init only"

    lib.write(obj, 1, 6, False)
    assert lib.call(obj, "Handle") == lib.OK
    assert lib.last_write(CFG1, 1) == b"\xbf", "driven low it is an output"

    lib.fail_at(3)
    assert lib.call(obj, "Handle") == lib.ERR, "and its write reports the bus"


@pytest.mark.parametrize("nth", [1, 2])
def test_ALX1564_P777_the_interrupt_handler_stops_at_the_read_that_fails(pca9539a_lib, expander, nth):
    lib, obj = expander
    lib.fail_at(nth)

    assert lib.call(obj, "irqHandle") == lib.ERR


def test_ALX1564_P778_the_pin_operations_of_port_1_and_port_0_each_change_their_own_register(pca9539a_lib, expander):
    """Write low on port 1 and toggle on port 0 - the two halves P611 left to the other port."""
    lib, obj = expander
    lib.write(obj, 1, 2, True)
    lib.write(obj, 1, 2, False)
    lib.pin_op(obj, "Toggle", 0, 1)

    assert lib.call(obj, "Handle") == lib.OK
    assert lib.last_write(OUT1, 1) == b"\x00"
    assert lib.last_write(OUT0, 1) == b"\x02"


@pytest.mark.expect_assert("P779: a port outside 0 and 1 asserts in Read, Read_Port_Raw, Write and Toggle")
def test_ALX1564_P779_a_port_the_part_does_not_have_asserts_and_changes_nothing(pca9539a_lib, expander):
    """CHARACTERIZATION: a read answers false and the raw port 0; a write and a toggle touch no register."""
    lib, obj = expander

    assert lib.read(obj, 2, 0) is False
    assert lib.read_port(obj, 2) == 0
    lib.write(obj, 2, 0, True)
    lib.pin_op(obj, "Toggle", 2, 0)
    assert lib.call(obj, "Handle") == lib.OK
    assert (lib.last_write(OUT0, 1), lib.last_write(OUT1, 1)) == (b"\x00", b"\x00")


@pytest.mark.expect_assert("P779: the library's weak RegStruct_SetVal asserts - a product implements it")
def test_ALX1564_P779_the_weak_register_hook_asserts_that_a_product_implements_it(pca9539a_lib):
    """CHARACTERIZATION: the hook Init no longer calls; the library's own body is one assertion."""
    obj = pca9539a_lib.new()

    pca9539a_lib.weak_set_val(obj)
