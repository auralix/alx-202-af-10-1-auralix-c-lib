"""Auralix C Library - AlxPcal6416a on the host.

A 16-pin IO expander: the PCA9539A's two ports plus drive strength, input latch, pull resistors
and interrupt masks. The driver keeps a shadow of every register; Init writes its defaults (the
helper, as the application, keeps them all), Handle does exactly the port transfers its caller
names, and a tri-state read reads the pin twice, under a pull-up and under a pull-down.

Not proven here: the floating case of the tri-state read (HiZ), whose input bit follows the
pull - the I2C fake holds one value per register and cannot answer two reads differently.

Proofs (ALX-1564, A92): P615-P621.
"""

import pytest

IN0, IN1, OUT0, OUT1 = 0x00, 0x01, 0x02, 0x03
PULL_EN0, PULL_SEL0 = 0x46, 0x48
DEFAULTS = {
    0x04: 0x00, 0x05: 0x00,                        # no polarity inversion
    0x06: 0xFF, 0x07: 0xFF,                        # every pin an input
    0x40: 0xFF, 0x41: 0xFF, 0x42: 0xFF, 0x43: 0xFF,  # full drive strength
    0x44: 0x00, 0x45: 0x00,                        # no input latch
    0x46: 0x00, 0x47: 0x00,                        # no pull resistor
    0x48: 0xFF, 0x49: 0xFF,                        # pull-ups selected, for when one is enabled
    0x4A: 0xFF, 0x4B: 0xFF,                        # every interrupt masked
    0x4F: 0x00,                                    # push-pull outputs
}


@pytest.fixture
def expander(pcal6416a_lib):
    """One expander, its bus and the part initialised."""
    obj = pcal6416a_lib.new()
    assert pcal6416a_lib.call(obj, "Init") == pcal6416a_lib.OK
    return pcal6416a_lib, obj


def test_ALX1564_P615_init_writes_every_configuration_register_once_with_its_default(pcal6416a_lib):
    """Seventeen registers, each once, with the driver's defaults - and no port is written.

    The outputs are left to the application's first Handle, so a product decides its outputs
    before any pin becomes one.
    """
    obj = pcal6416a_lib.new()

    assert pcal6416a_lib.call(obj, "Init") == pcal6416a_lib.OK

    assert {r: pcal6416a_lib.last_write(r, 1)[0] for r in DEFAULTS} == DEFAULTS
    assert {r: pcal6416a_lib.write_count(r) for r in DEFAULTS} == dict.fromkeys(DEFAULTS, 1)
    assert [pcal6416a_lib.write_count(r) for r in (IN0, IN1, OUT0, OUT1)] == [0, 0, 0, 0]


@pytest.mark.parametrize(("in0", "in1", "out0", "out1"), [
    (True, False, False, False),
    (False, True, False, False),
    (False, False, True, False),
    (False, False, False, True),
    (True, True, True, True),
])
def test_ALX1564_P616_handle_does_exactly_the_transfers_it_is_asked_for(expander, in0, in1, out0, out1):
    """Each of the four flags is one transfer, and a flag that is off is no traffic at all."""
    lib, obj = expander

    assert lib.handle(obj, in0=in0, in1=in1, out0=out0, out1=out1) == lib.OK

    assert [lib.read_count(IN0), lib.read_count(IN1)] == [int(in0), int(in1)]
    assert [lib.write_count(OUT0), lib.write_count(OUT1)] == [int(out0), int(out1)]


def test_ALX1564_P617_a_pin_reads_its_input_bit_as_of_the_last_read(expander):
    """A read answers from the shadow, so a change on the part shows only after the next read."""
    lib, obj = expander
    lib.set_register(IN0, bytes([0b0100_0000]))
    lib.set_register(IN1, bytes([0b0000_0010]))
    lib.handle(obj, in0=True, in1=True)

    assert [lib.read(obj, 0, p) for p in range(8)] == [False] * 6 + [True, False]
    assert [lib.read(obj, 1, p) for p in range(8)] == [False, True] + [False] * 6

    lib.set_register(IN0, bytes([0x00]))
    assert lib.read(obj, 0, 6) is True, "a read went to the part instead of the shadow"


def test_ALX1564_P618_pin_operations_change_only_their_own_bit_and_wait_for_handle(expander):
    """Set, Reset, Toggle and Write are bits of the output shadow; Handle sends them."""
    lib, obj = expander
    lib.pin_op(obj, "Set", 0, 1)
    lib.pin_op(obj, "Set", 0, 6)
    lib.pin_op(obj, "Reset", 0, 1)
    lib.pin_op(obj, "Toggle", 1, 3)
    lib.write(obj, 1, 7, True)
    assert lib.write_count(OUT0) + lib.write_count(OUT1) == 0, "a pin operation reached the part before Handle"

    lib.handle(obj, out0=True, out1=True)

    assert (lib.last_write(OUT0, 1), lib.last_write(OUT1, 1)) == (bytes([0b0100_0000]), bytes([0b1000_1000]))


@pytest.mark.parametrize(("level", "expected"), [(True, "Hi"), (False, "Lo")])
def test_ALX1564_P619_a_driven_pin_reads_the_same_under_both_pulls(expander, level, expected):
    """A pin something drives does not follow the pull: high under both is Hi, low under both Lo."""
    lib, obj = expander
    lib.set_register(IN0, bytes([0b0000_1000 if level else 0x00]))

    assert lib.read_tri_state(obj, 0, 3) == (lib.OK, expected)


def test_ALX1564_P620_a_tri_state_read_leaves_the_pins_pull_disabled(expander):
    """Up, off, down, off: the pull registers are written four times and end with no pull.

    A pull left on would load the line the pin reads, on every board, from the first tri-state
    read on.
    """
    lib, obj = expander
    writes_before = lib.write_count(PULL_EN0)

    lib.read_tri_state(obj, 0, 3)

    assert lib.write_count(PULL_EN0) - writes_before == 4
    assert lib.last_write(PULL_EN0, 1) == b"\x00"
    assert (lib.read_count(IN0), lib.read_count(IN1)) == (2, 2)


def test_ALX1564_P621_init_fails_when_the_part_does_not_answer(pcal6416a_lib):
    """No acknowledge: Init reports the error and configures nothing."""
    obj = pcal6416a_lib.new()
    pcal6416a_lib.slave_ready(False)

    assert pcal6416a_lib.call(obj, "Init") == pcal6416a_lib.ERR

    assert sum(pcal6416a_lib.write_count(r) for r in DEFAULTS) == 0
