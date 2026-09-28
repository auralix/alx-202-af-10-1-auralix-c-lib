"""Auralix C Library - AlxLin, the master's publish path, on the host.

A LIN frame is a break, a sync byte, a protected identifier and then the data with a checksum after
it. Two of those are computed rather than given - the identifier's two parity bits and the checksum
- and both are defined by the LIN specification rather than by this library, which is what makes
them worth checking from outside: the expected values here come from the specification's own
formulas and its own published examples, not from reading the implementation.

The serial port is faked, so what a test reads back is the BYTES the master put on the wire, which
is the only place a protected identifier or a checksum exists at all.

Only the PUBLISH path is covered. It is the one that puts a whole frame out in a single call; the
subscribe half waits on the clock and belongs with a self-advancing tick.

Proofs (ALX-1553): P457-P465.
"""

import pytest

SYNC = 0x55
ID_MASTER_REQ = 0x3C            # the specification's diagnostic pair, and the one everybody knows
ID_SLAVE_RESP = 0x3D


def _pid(frame_id: int) -> int:
    """The protected identifier, from the LIN specification's formula rather than the library's.

    P0 = ID0 xor ID1 xor ID2 xor ID4, P1 = NOT (ID1 xor ID3 xor ID4 xor ID5), and the two sit in
    bits 6 and 7 above the six identifier bits.
    """
    bit = [(frame_id >> i) & 1 for i in range(6)]
    p0 = bit[0] ^ bit[1] ^ bit[2] ^ bit[4]
    p1 = 1 - (bit[1] ^ bit[3] ^ bit[4] ^ bit[5])
    return (frame_id & 0x3F) | (p0 << 6) | (p1 << 7)


def _checksum(data: bytes, pid: int = 0) -> int:
    """The specification's checksum: sum the bytes with END-AROUND CARRY, then invert.

    The classic checksum sums the data only; the enhanced one starts from the protected identifier.
    Passing pid = 0 is therefore the classic one, which is also how the library writes it.
    """
    total = pid
    for byte in data:
        total += byte
        if total >= 256:
            total -= 255            # add the carry back in, which is what makes it end-around
    return (~total) & 0xFF


@pytest.fixture
def master(lin_lib):
    """An initialised LIN master with an empty wire."""
    lib = lin_lib
    return lib, lib.new()


def test_ALX1553_P457_a_frame_is_sync_then_the_identifier_then_the_data_and_a_checksum(master):
    """The shape of a frame, and its length: two header bytes, the data, one checksum.

    Everything else in this file is about the two computed bytes; this is the one that says where
    they sit. A frame assembled in the wrong order would still contain all the right values.
    """
    lib, handle = master
    data = bytes([0x11, 0x22, 0x33])

    assert lib.publish(handle, 0x01, data) == 0

    wire = lib.wire(handle)
    assert len(wire) == 2 + len(data) + 1
    assert wire[0] == SYNC
    assert wire[1] == _pid(0x01)
    assert wire[2:-1] == data
    assert wire[-1] == _checksum(data)


@pytest.mark.parametrize("frame_id", range(64))
def test_ALX1553_P458_the_protected_identifier_carries_the_specifications_parity(master, frame_id):
    """All sixty-four identifiers, each against the specification's own parity formula.

    Exhaustive because parity is exactly the kind of thing that is right for the identifiers
    somebody tried and wrong for one of the others, and a slave that sees a bad parity is required
    to ignore the frame - so the symptom is a device that answers 63 identifiers and not the 64th.
    """
    lib, handle = master

    lib.publish(handle, frame_id, bytes([0x00]))

    assert lib.wire(handle)[1] == _pid(frame_id)


def test_ALX1553_P459_the_two_diagnostic_identifiers_have_the_values_everyone_quotes(master):
    """0x3C is 0x3C and 0x3D is 0x7D - the master request and slave response frames.

    Written as literals rather than through the formula, because these two are the ones that appear
    on every LIN analyser and in every specification example. If the formula in this file and the
    library were both wrong in the same way, these two would still catch it.
    """
    lib, handle = master

    lib.publish(handle, ID_MASTER_REQ, bytes([0x00]))
    assert lib.wire(handle)[1] == 0x3C

    lib.publish(handle, ID_SLAVE_RESP, bytes([0x00]))
    assert lib.wire(handle)[1] == 0x7D


@pytest.mark.parametrize("data", [
    bytes([0x01]),
    bytes([0x01, 0x02, 0x03]),
    bytes([0x00] * 8),
    bytes([0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88]),
])
def test_ALX1553_P460_the_classic_checksum_covers_the_data_and_nothing_else(master, data):
    """The classic checksum is the inverted end-around sum of the data bytes alone.

    "Alone" is the half worth checking: the same data published under a different identifier must
    produce the same classic checksum, which is what distinguishes it from the enhanced one.
    """
    lib, handle = master

    lib.publish(handle, 0x01, data)
    first = lib.wire(handle)[-1]

    lib.publish(handle, 0x20, data)
    second = lib.wire(handle)[-1]

    assert first == _checksum(data)
    assert second == first, "the classic checksum changed with the identifier"


@pytest.mark.parametrize("frame_id", [0x00, 0x01, 0x20, 0x3C, 0x3D])
def test_ALX1553_P461_the_enhanced_checksum_starts_from_the_protected_identifier(master, frame_id):
    """The enhanced checksum includes the PID, so the same data under two identifiers differs.

    That is the entire difference between the two, and getting it wrong produces a frame every
    slave rejects - which looks like a wiring fault rather than a software one.
    """
    lib, handle = master
    data = bytes([0x01, 0x02, 0x03])

    lib.publish(handle, frame_id, data, enhanced=True)

    assert lib.wire(handle)[-1] == _checksum(data, _pid(frame_id))


def test_ALX1553_P462_the_checksum_carries_around_rather_than_truncating(master):
    """Two bytes of 0xFF: the sum overflows, and the carry comes back in rather than being lost.

    This is the case that separates the specification's checksum from an ordinary one-byte sum, and
    the one a hand-written implementation gets wrong. 0xFF + 0xFF end-around is 0xFF, inverted is
    0x00 - a checksum of zero, which is also what an implementation that truncated to nothing would
    return, so the surrounding bytes are checked with it.
    """
    lib, handle = master
    data = bytes([0xFF, 0xFF])

    lib.publish(handle, 0x02, data)

    wire = lib.wire(handle)
    assert wire[-1] == 0x00
    assert wire == bytes([SYNC, _pid(0x02), 0xFF, 0xFF, 0x00])


def test_ALX1553_P463_the_break_is_the_serial_ports_job_and_is_not_a_byte_here(master):
    """CHARACTERIZATION: nothing on this wire is the break field, and nothing here can prove one.

    A LIN frame opens with a break - a dominant level longer than any byte - and the library does
    not write one: the comment beside the frame assembly says "Send by AlxSerialPort_Write", so it
    is the UART driver's break generation that produces it. This fake records bytes, so a host test
    sees a frame starting at the sync byte and CANNOT say whether a device would send the break.

    Worth writing down rather than leaving as an absence: a reader who sees a frame begin at 0x55
    might reasonably think the break was forgotten, and someone porting this to a port whose Write
    does not generate a break would find every slave ignoring every frame, with nothing here to
    have warned them.
    """
    lib, handle = master

    lib.publish(handle, 0x01, bytes([0x00]))

    assert lib.wire(handle)[0] == SYNC, "something precedes the sync byte after all"


def test_ALX1553_P464_publishing_a_frame_discards_one_that_had_not_gone_out_yet(master):
    """CHARACTERIZATION: each publish FLUSHES the transmit side first, so it can destroy a frame.

    AlxLin_Master_Publish calls AlxSerialPort_FlushTxFifo before it writes. On a port whose
    transmit is interrupt-driven - which is what this library's serial ports are - anything still
    queued is thrown away, so a master that publishes twice before the first frame has gone out
    puts only the SECOND one on the bus. Measured: two publishes, one frame on the wire.

    Both calls return Alx_Ok, and nothing anywhere says a frame was lost. A LIN master normally
    owns the bus and publishes on a schedule with room between frames, so in service this does not
    happen - but it is not enforced, the failure is silent, and the symptom on a bus is a slave
    that never sees the frame it was waiting for.

    Asserted as it behaves rather than as it ought to, because flushing before a frame is a
    defensible thing for a bus master to want. What was not written down anywhere is the cost.
    """
    lib, handle = master
    first = bytes([0xAA])
    second = bytes([0xBB, 0xCC])

    lib.publish(handle, 0x01, first)
    lib.publish(handle, 0x02, second)

    wire = lib.wire(handle)
    assert wire == bytes([SYNC, _pid(0x02), *second, _checksum(second)]), (
        "the flush behaviour changed - a second frame no longer replaces the first"
    )
    assert bytes([*first]) not in wire, "the first frame survived, which is what it should do"


def test_ALX1553_P465_a_frame_read_off_the_wire_leaves_the_next_one_alone(master):
    """The complement of P464: with the wire emptied between them, both frames are whole.

    Which is what a bus does when it has actually sent the first frame - so this is the ordinary
    case, and having both tests next to each other is what makes P464 read as a cost rather than
    as a bug report.
    """
    lib, handle = master
    first = bytes([0xAA])
    second = bytes([0xBB, 0xCC])

    lib.publish(handle, 0x01, first)
    assert lib.wire(handle) == bytes([SYNC, _pid(0x01), *first, _checksum(first)])

    lib.publish(handle, 0x02, second)
    assert lib.wire(handle) == bytes([SYNC, _pid(0x02), *second, _checksum(second)])
