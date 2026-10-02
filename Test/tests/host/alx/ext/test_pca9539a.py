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
