"""ALX-1553 - alxPi4ioe5v6534q PC unit tests (Tier 2 over a faked I2C bus and a faked pin).

A 40-channel IO expander, and the device driver a product touches more than any other: the board
this suite was written for reaches a third of its digital IO through one. What the driver owns is a
mapping from (port, pin) to a bit in a five-byte shadow register, and the shape of the two bulk
transfers that exchange that shadow with the part. Both are the kind of thing that is either right
or wrong and never nearly right - a swapped byte drives the wrong relay - and neither had a test.

The mapping is checked EXHAUSTIVELY, all forty channels, because forty is small and the failure it
guards against is a single wrong bit somewhere in the middle of the range.

As in the current sensor group, the driver's register configuration hook is WEAK and this suite
cannot override it on a COFF host, so the library's own default runs and the registers written here
are the driver's defaults. That is visible in P233, and it is worth seeing.

Test group P228-P234 = ALX-1553 IO expander proofs.
"""

import pytest

pytestmark = pytest.mark.unit

PORTS, PINS = 5, 8
ALL_CHANNELS = [(port, pin) for port in range(PORTS) for pin in range(PINS)]


@pytest.fixture
def dev(pi4ioe_lib):
    """An initialised expander with every channel driven low, so a set bit is one the test put there."""
    device = pi4ioe_lib.expander()
    pi4ioe_lib.clear_all(device)
    return device


# =====================================================================
# P228-P229 - the mapping, and the four ways to change a pin
# =====================================================================


@pytest.mark.parametrize(("port", "pin"), ALL_CHANNELS)
def test_ALX1553_P228_each_channel_is_one_bit_of_one_byte(pi4ioe_lib, dev, port, pin):
    """All forty channels, one at a time: exactly one bit changes, in the byte of its own port.

    The whole shadow register is compared, not just the byte in question, so a write that reached
    the right bit of the WRONG port fails here rather than being hidden by a test that only looked
    where it expected to find it.
    """
    _, out_addr = pi4ioe_lib.addresses(dev)
    pi4ioe_lib.write(dev, port, pin, True)
    assert pi4ioe_lib.handle(dev) == pi4ioe_lib.OK

    expected = bytearray(PORTS)
    expected[port] = 1 << pin
    assert pi4ioe_lib.outputs_on_the_bus(out_addr) == bytes(expected)


@pytest.mark.parametrize(("port", "pin"), [(0, 0), (2, 5), (4, 7)])
def test_ALX1553_P229_set_reset_and_toggle_agree_with_write(pi4ioe_lib, dev, port, pin):
    """Four entry points, one bit: the convenience calls do exactly what Write does.

    Toggle is checked from both sides, because a toggle implemented as "set if clear" would pass a
    test that only ever toggled a zero.
    """
    _, out_addr = pi4ioe_lib.addresses(dev)
    high = bytearray(PORTS)
    high[port] = 1 << pin
    low = bytes(PORTS)

    pi4ioe_lib.act(dev, "Set", port, pin)
    pi4ioe_lib.handle(dev)
    assert pi4ioe_lib.outputs_on_the_bus(out_addr) == bytes(high), "Set"

    pi4ioe_lib.act(dev, "Reset", port, pin)
    pi4ioe_lib.handle(dev)
    assert pi4ioe_lib.outputs_on_the_bus(out_addr) == low, "Reset"

    pi4ioe_lib.act(dev, "Toggle", port, pin)
    pi4ioe_lib.handle(dev)
    assert pi4ioe_lib.outputs_on_the_bus(out_addr) == bytes(high), "Toggle up"

    pi4ioe_lib.act(dev, "Toggle", port, pin)
    pi4ioe_lib.handle(dev)
    assert pi4ioe_lib.outputs_on_the_bus(out_addr) == low, "Toggle down"


# =====================================================================
# P230 - the way in
# =====================================================================


@pytest.mark.parametrize(("port", "pin"), ALL_CHANNELS)
def test_ALX1553_P230_a_read_reflects_the_bit_the_part_sent(pi4ioe_lib, dev, port, pin):
    """The same forty channels in the other direction, against a pattern that is not symmetric.

    The input bytes are 0x01 0x02 0x04 0x08 0x10 - a different single bit per port - so a driver
    that read the right bit of the wrong byte would answer false everywhere except by accident.
    """
    in_addr, _ = pi4ioe_lib.addresses(dev)
    pattern = bytes(1 << port_index for port_index in range(PORTS))
    pi4ioe_lib.set_inputs(in_addr, pattern)
    assert pi4ioe_lib.handle(dev) == pi4ioe_lib.OK

    expected = bool(pattern[port] & (1 << pin))
    assert pi4ioe_lib.read(dev, port, pin) is expected


# =====================================================================
# P231-P232 - the transfers
# =====================================================================


@pytest.mark.parametrize(("in_ports", "out_ports"), [(5, 5), (5, 0), (0, 5), (0, 0), (1, 1), (3, 2)])
def test_ALX1553_P231_handle_transfers_exactly_the_ports_it_is_asked_for(pi4ioe_lib, dev,
                                                                        in_ports, out_ports):
    """A count of zero means no transfer at all, not a transfer of zero bytes.

    The argument is a number of PORTS, and a product uses it to skip an unused half of the device -
    so "zero means silence" is the property that makes it worth having, and the one an implementation
    would break by passing the count straight to the bus.
    """
    in_addr, out_addr = pi4ioe_lib.addresses(dev)
    reads_before = pi4ioe_lib.bus_reads(in_addr)
    writes_before = pi4ioe_lib.bus_writes(out_addr)

    assert pi4ioe_lib.handle(dev, in_ports, out_ports) == pi4ioe_lib.OK

    assert pi4ioe_lib.bus_reads(in_addr) == reads_before + (1 if in_ports else 0)
    assert pi4ioe_lib.bus_writes(out_addr) == writes_before + (1 if out_ports else 0)


def test_ALX1553_P232_a_bus_failure_stops_the_pass_and_is_reported(pi4ioe_lib, dev):
    """When the read fails the write must not happen: the driver returns instead of carrying on.

    A service pass that half-completed and reported success would leave the caller believing the
    outputs it set are on the part when they are not, which on an IO expander means a relay that did
    not move and nothing saying so.
    """
    in_addr, out_addr = pi4ioe_lib.addresses(dev)
    writes_before = pi4ioe_lib.bus_writes(out_addr)

    pi4ioe_lib.c.AlxI2cFake_SetForcedStatus(pi4ioe_lib.ERR)
    assert pi4ioe_lib.handle(dev) == pi4ioe_lib.ERR, "the failure was not reported"
    pi4ioe_lib.c.AlxI2cFake_SetForcedStatus(pi4ioe_lib.OK)

    assert pi4ioe_lib.bus_writes(out_addr) == writes_before, "the write ran after the read failed"
    assert pi4ioe_lib.handle(dev) == pi4ioe_lib.OK, "and the driver still works afterwards"
    assert pi4ioe_lib.bus_reads(in_addr) > 0


# =====================================================================
# P233-P234 - what initialisation leaves behind
# =====================================================================


def test_ALX1553_P233_every_channel_is_driven_high_after_initialisation(pi4ioe_lib):
    """CHARACTERIZATION: the output shadow starts as all ones, so the first pass drives all forty.

    That is the part's own reset state for this register, and the driver keeps it - but it means the
    level a channel takes between `Init` and a product's first deliberate write is HIGH, on all
    forty, for as long as that gap lasts. Whether that is safe is a board question, not a library
    one; what this test does is make sure nobody has to guess which it is.

    Measured with the library's own weak configuration default in place - see the file docstring -
    so a product that overrides it may see something else, and this is the baseline it overrides.
    """
    device = pi4ioe_lib.expander()          # deliberately NOT cleared
    _, out_addr = pi4ioe_lib.addresses(device)
    assert pi4ioe_lib.handle(device) == pi4ioe_lib.OK
    assert pi4ioe_lib.outputs_on_the_bus(out_addr) == b"\xff" * PORTS


def test_ALX1553_P234_the_reset_pin_is_driven_once_and_left_released(pi4ioe_lib):
    """The part is held in reset and let go during Init, and not touched again.

    Counted rather than inspected: a driver that toggled the reset pin on every service pass would
    still leave it released at the end, so the level alone proves nothing.
    """
    device = pi4ioe_lib.expander()
    level, writes = pi4ioe_lib.reset_pin(device)
    assert level is True, "the part was left held in reset"
    assert writes >= 1, "the reset pin was never driven at all"

    for _ in range(5):
        pi4ioe_lib.handle(device)
    assert pi4ioe_lib.reset_pin(device) == (True, writes), "a service pass touched the reset pin"
