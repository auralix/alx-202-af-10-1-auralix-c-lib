"""Auralix C Library - AlxLin's receive paths on the host: the master's subscribe, the slave, the RX buffer.

test_lin.py covers the publish path, the one call that puts a whole frame out. This is the other
half. A master SUBSCRIBES by sending a header and reading the slave's response back off the bus,
either blocking (Master_Subscribe reads the serial port's RX FIFO after a wait) or byte by byte
through the RX buffer (SubscribeViaCallback, then RxBuff_Handle per received byte, then the
weak callbacks). A slave receives headers the same byte-by-byte way and answers the ones its frame
table says it publishes, or reads the master's diagnostic request addressed to its NAD.

The serial port is faked: a test injects the bytes the bus would carry and reads back what the
object put on it. The host's tick stands still unless a test advances it, which is how a response
timeout is made to happen. The weak callbacks stay the library's own, so what a test observes is
the frame the RX buffer holds, the response on the wire and the slave-request flag.

Proofs (ALX-1564, A97): P686-P698, P707.
"""

import pytest

from tests.host.alx.test_lin import _checksum, _pid

SYNC = 0x55
ID_MASTER_REQ, ID_SLAVE_RESP = 0x3C, 0x3D
TIMEOUT_MS = 100            # the helper constructs every object with a 100 ms response timeout


def _response(frame_id: int, data: bytes, *, enhanced: bool = False) -> bytes:
    """What the bus carries for a frame: the sync, the protected id, the data and its checksum.

    The helper constructs the object with breakSyncOffset 1, so one byte - the sync - stands
    before the protected identifier in what the object receives; the break is no byte.
    """
    pid = _pid(frame_id)
    return bytes([SYNC, pid, *data, _checksum(data, pid if enhanced else 0)])


@pytest.fixture
def slave(lin_lib):
    """A slave, initialised, with a three-frame table: two it subscribes to and one it publishes."""
    handle = lin_lib.new_slave()
    assert lin_lib.slave_init(handle) == 0
    lin_lib.slave_frames(handle, [(0x10, 2, False, False), (0x20, 4, True, False), (0x30, 8, False, True)])
    return lin_lib, handle


# =====================================================================
# The master's blocking subscribe
# =====================================================================


@pytest.mark.parametrize("enhanced", [False, True])
def test_ALX1564_P686_a_master_subscribes_by_sending_a_header_and_reading_the_response(lin_lib, enhanced):
    """The header goes out as sync and protected id; the response comes back parsed and checked."""
    handle = lin_lib.new()
    data = bytes([0x12, 0x34, 0x56, 0x78])
    lin_lib.respond(handle, _response(0x21, data, enhanced=enhanced))

    status, pid, got, checksum = lin_lib.subscribe(handle, 0x21, 4, enhanced=enhanced)

    assert status == 0
    assert lin_lib.wire(handle) == bytes([SYNC, _pid(0x21)]), "the header, nothing more"
    assert (pid, got, checksum) == (_pid(0x21), data, _checksum(data, _pid(0x21) if enhanced else 0))


def test_ALX1564_P687_a_master_refuses_a_response_of_the_wrong_length_identifier_or_checksum(lin_lib):
    """Each check in the order the master makes them; the last one both ways of the checksum."""
    handle = lin_lib.new()
    data = bytes([0xAA, 0xBB])

    assert lin_lib.subscribe(handle, 0x05, 2)[0] == 1, "nobody answered: the length cannot match"

    lin_lib.respond(handle, _response(0x05, data) + b"\x00")
    assert lin_lib.subscribe(handle, 0x05, 2)[0] == 1, "one byte too many"

    lin_lib.respond(handle, _response(0x06, data))
    assert lin_lib.subscribe(handle, 0x05, 2)[0] == 1, "another frame's identifier"

    lin_lib.respond(handle, _response(0x05, data, enhanced=True))
    assert lin_lib.subscribe(handle, 0x05, 2)[0] == 1, "an enhanced checksum read as classic"

    lin_lib.respond(handle, _response(0x05, data))
    assert lin_lib.subscribe(handle, 0x05, 2, enhanced=True)[0] == 1, "a classic checksum read as enhanced"

    lin_lib.respond(handle, _response(0x05, data))
    lin_lib.port_fail(1, lin_lib.READ)
    assert lin_lib.subscribe(handle, 0x05, 2)[0] == 1, "the port would not read"
    lin_lib.port_fail(1, lin_lib.WRITE)
    assert lin_lib.subscribe(handle, 0x05, 2)[0] == 1, "the port would not send the header"


# =====================================================================
# The master's subscribe through the RX buffer
# =====================================================================


def test_ALX1564_P688_a_master_subscribing_via_callback_sends_the_header_and_collects_the_response(lin_lib):
    """The header goes out at once; the response arrives a byte at a time and ends in the buffer."""
    handle = lin_lib.new()
    data = bytes([0x01, 0x02])

    assert lin_lib.subscribe_via_callback(handle, 0x10, 2) == 0
    assert lin_lib.wire(handle) == bytes([SYNC, _pid(0x10)])
    assert lin_lib.rx_active(handle) is True
    assert lin_lib.rx_frame(handle)["pid"] == _pid(0x10)

    lin_lib.rx_bytes(handle, _response(0x10, data))

    assert lin_lib.rx_active(handle) is False, "a complete, correct response closes the frame"
    frame = lin_lib.rx_frame(handle)
    assert (frame["id"], frame["len"], frame["data"], frame["checksum"]) == (0x10, 2, data, _checksum(data))

    lin_lib.rx_bytes(handle, b"\x55")
    assert lin_lib.rx_index(handle) == len(_response(0x10, data)), "an inactive buffer ignores the bus"

    lin_lib.port_fail(1, lin_lib.WRITE)
    assert lin_lib.subscribe_via_callback(handle, 0x10, 2) == 1, "the port would not send the header"


@pytest.mark.parametrize("enhanced", [False, True])
def test_ALX1564_P689_a_response_with_the_wrong_identifier_or_checksum_closes_the_frame_unread(lin_lib, enhanced):
    """The identifier is checked when it arrives, the checksum when it arrives; either ends the frame."""
    handle = lin_lib.new()
    data = bytes([0x0F, 0xF0])

    lin_lib.subscribe_via_callback(handle, 0x11, 2, enhanced=enhanced)
    lin_lib.rx_bytes(handle, _response(0x12, data, enhanced=enhanced))
    assert lin_lib.rx_active(handle) is False
    assert lin_lib.rx_frame(handle)["data"] == b"\x00\x00", "another frame's response was not taken"

    lin_lib.subscribe_via_callback(handle, 0x11, 2, enhanced=enhanced)
    lin_lib.rx_bytes(handle, _response(0x11, data, enhanced=not enhanced))
    assert lin_lib.rx_active(handle) is False
    assert lin_lib.rx_frame(handle)["checksum"] == 0, "the wrong checksum was not kept"


def test_ALX1564_P690_the_masters_handle_times_a_response_out_after_the_configured_wait(lin_lib):
    """Nothing happens while the response is in time; one tick past the timeout closes the frame."""
    handle = lin_lib.new()
    lin_lib.subscribe_via_callback(handle, 0x10, 2)

    lin_lib.tick_ms(TIMEOUT_MS - 1)
    lin_lib.handle(handle, master=True)
    assert lin_lib.rx_active(handle) is True

    lin_lib.tick_ms(2)
    lin_lib.handle(handle, master=True)
    assert lin_lib.rx_active(handle) is False

    lin_lib.handle(handle, master=True)
    assert lin_lib.rx_active(handle) is False, "a closed frame stays closed"


# =====================================================================
# Init, DeInit and the serial IRQ
# =====================================================================


def test_ALX1564_P691_init_and_deinit_follow_the_serial_port_on_both_sides(lin_lib):
    """A master or a slave is initialised when its port is, and not when the port refuses.

    An object is a master from Master_Init to Master_DeInit and Master_IsInit asserts that, so it
    is asked only in between; a constructed object is a slave until then, and Slave_IsInit is
    asked of it before and after.
    """
    master = lin_lib.new(init=False)
    assert lin_lib.init(master) == 0
    assert lin_lib.is_init(master) is True
    assert lin_lib.deinit(master) == 0
    assert lin_lib.slave_is_init(master) is False, "no longer a master, not initialised either"

    slave = lin_lib.new_slave()
    assert lin_lib.slave_is_init(slave) is False
    assert lin_lib.slave_init(slave) == 0
    assert lin_lib.slave_is_init(slave) is True
    lin_lib.irq(slave)
    assert lin_lib.slave_deinit(slave) == 0
    assert lin_lib.slave_is_init(slave) is False

    lin_lib.port_fail(1, lin_lib.INIT)
    assert lin_lib.init(master) == 1
    assert lin_lib.slave_init(slave) == 1
    assert (lin_lib.slave_is_init(master), lin_lib.slave_is_init(slave)) == (False, False)

    lin_lib.port_fail(0, 0)
    assert (lin_lib.init(master), lin_lib.slave_init(slave)) == (0, 0)
    lin_lib.port_fail(1, lin_lib.DEINIT)
    assert lin_lib.deinit(master) == 1
    assert lin_lib.slave_deinit(slave) == 1
    assert (lin_lib.is_init(master), lin_lib.slave_is_init(slave)) == (True, True), "still initialised"


# =====================================================================
# The slave's blocking subscribe
# =====================================================================


@pytest.mark.parametrize("enhanced", [False, True])
def test_ALX1564_P692_a_slave_subscribes_to_whatever_frame_the_bus_carries(slave, enhanced):
    """The slave does not know the identifier in advance: it reads it, and its parity, off the bus."""
    lib, handle = slave
    data = bytes([0xDE, 0xAD, 0xBE, 0xEF])
    lib.respond(handle, _response(0x2A, data, enhanced=enhanced))

    status, frame_id, pid, got, checksum = lib.slave_subscribe(handle, 4, enhanced=enhanced)

    assert status == 0
    assert (frame_id, pid, got, checksum) == (0x2A, _pid(0x2A), data, _checksum(data, _pid(0x2A) if enhanced else 0))


def test_ALX1564_P693_a_slave_gives_up_on_a_frame_that_is_late_too_long_or_wrong(slave):
    """A short frame times out, a long one is refused at once, and the two checks are the master's."""
    lib, handle = slave
    data = bytes([0x11, 0x22])

    lib.respond(handle, _response(0x10, data)[:-1])
    assert lib.slave_subscribe(handle, 2)[0] == 1, "one byte short, and the wait ran out"

    lib.respond(handle, _response(0x10, data) + b"\x00")
    assert lib.slave_subscribe(handle, 2)[0] == 1, "one byte too many"

    bad_pid = bytes([SYNC, _pid(0x10) ^ 0x80, *data, _checksum(data)])
    lib.respond(handle,bad_pid)
    assert lib.slave_subscribe(handle, 2)[0] == 1, "the parity does not match the identifier"

    lib.respond(handle, _response(0x10, data, enhanced=True))
    assert lib.slave_subscribe(handle, 2)[0] == 1, "an enhanced checksum read as classic"

    lib.respond(handle, _response(0x10, data))
    lib.port_fail(1, lib.READ)
    assert lib.slave_subscribe(handle, 2)[0] == 1, "the port would not read"


def test_ALX1564_P698_a_slave_keeps_waiting_while_the_frame_is_still_coming(slave):
    """Nothing there at the first look, the whole frame at the second: the wait goes round once."""
    lib, handle = slave
    data = bytes([0x33, 0x44])
    lib.respond_late(handle, _response(0x10, data), 2)

    status, frame_id, _, got, _ = lib.slave_subscribe(handle, 2, timeout_ms=1000)

    assert (status, frame_id, got) == (0, 0x10, data)


def test_ALX1564_P707_a_master_reports_a_port_that_would_not_send_its_frame(lin_lib):
    """Publish puts the whole frame out in one write; a refused write is the status it returns."""
    handle = lin_lib.new()
    lin_lib.port_fail(1, lin_lib.WRITE)

    assert lin_lib.publish(handle, 0x10, b"\x01\x02") == 1
    assert lin_lib.wire(handle) == b""


# =====================================================================
# The slave's RX buffer
# =====================================================================


def test_ALX1564_P694_a_slave_answers_a_header_of_a_frame_it_publishes(slave):
    """Its data, then the checksum the frame's configuration asks for; the buffer closes at once."""
    lib, handle = slave
    lib.rx_flush(handle)

    lib.rx_bytes(handle, bytes([SYNC, _pid(0x20)]))

    assert lib.rx_active(handle) is False
    assert lib.wire(handle) == bytes(4) + bytes([_checksum(bytes(4))]), "the weak callback leaves zeros"

    lib.slave_frames(handle, [(0x21, 2, True, True)])
    lib.rx_flush(handle)
    lib.rx_bytes(handle, bytes([SYNC, _pid(0x21)]))
    assert lib.wire(handle) == bytes(2) + bytes([_checksum(bytes(2), _pid(0x21))]), "enhanced, from the pid"

    lib.rx_flush(handle)
    lib.port_fail(1, lib.WRITE)
    lib.rx_bytes(handle, bytes([SYNC, _pid(0x21)]))
    assert lib.rx_active(handle) is False
    assert lib.wire(handle) == b"", "the port would not send: nothing on the wire, the frame closed"


def test_ALX1564_P695_a_slave_reads_a_frame_it_subscribes_to_and_ignores_what_it_does_not_know(slave):
    """A subscribed header opens a timed wait; the data and a good checksum fill the frame."""
    lib, handle = slave
    data = bytes([0xC0, 0xDE])
    lib.rx_flush(handle)
    assert lib.dbg_pin(handle) is False

    lib.rx_bytes(handle, bytes([SYNC, _pid(0x10)]))
    assert lib.rx_active(handle) is True
    assert lib.dbg_pin(handle) is True, "the debug pin marks the wait for the response"

    lib.rx_bytes(handle, bytes([*data, _checksum(data)]))
    assert lib.rx_active(handle) is False
    frame = lib.rx_frame(handle)
    assert (frame["id"], frame["len"], frame["data"], frame["enhanced"]) == (0x10, 2, data, False)

    lib.rx_flush(handle)
    assert lib.dbg_pin(handle) is False, "the flush lowers it"
    lib.rx_bytes(handle, bytes([SYNC, _pid(0x30), *bytes(8), _checksum(bytes(8), _pid(0x30))]))
    assert lib.rx_active(handle) is False
    assert lib.rx_frame(handle)["enhanced"] is True, "the table's checksum choice"

    lib.rx_flush(handle)
    lib.rx_bytes(handle, bytes([SYNC, _pid(0x10), *data, _checksum(data) ^ 0xFF]))
    assert lib.rx_active(handle) is False
    assert lib.rx_frame(handle)["checksum"] == 0, "a wrong checksum: the frame closed unread"

    lib.rx_flush(handle)
    lib.rx_bytes(handle, bytes([SYNC, _pid(0x10) ^ 0x40]))
    assert lib.rx_active(handle) is False, "wrong parity"

    lib.rx_flush(handle)
    lib.rx_bytes(handle, bytes([SYNC, _pid(0x15)]))
    assert lib.rx_active(handle) is False, "no entry in the table"

    lib.rx_flush(handle)
    lib.rx_bytes(handle, bytes([SYNC, _pid(0x10)]))
    lib.tick_ms(TIMEOUT_MS + 1)
    lib.handle(handle, master=False)
    assert lib.rx_active(handle) is False, "the response never came"

    bare = lib.new_slave(dbg_pin=False)
    lib.rx_flush(bare)
    lib.slave_frames(bare, [(0x10, 2, False, False)])
    lib.rx_bytes(bare, bytes([SYNC, _pid(0x10)]))
    assert lib.rx_active(bare) is True, "the same without a debug pin"


def test_ALX1564_P696_a_slave_takes_the_masters_request_addressed_to_it_or_to_everyone(slave):
    """The diagnostic request carries the NAD in its first byte; another node's is ignored."""
    lib, handle = slave
    lib.set_nad(handle, 0x10)
    assert lib.nad(handle) == 0x10
    request = bytes([0x10, 0x06, 0xB2, 0x00, 0x00, 0x00, 0x00, 0xFF])

    lib.rx_flush(handle)
    lib.rx_bytes(handle, bytes([SYNC, _pid(ID_MASTER_REQ), *request, _checksum(request)]))
    assert lib.rx_active(handle) is False
    assert lib.rx_frame(handle)["data"] == request, "the request is in the frame"
    assert lib.rx_index(handle) == 11, "every byte of the frame was taken, the callback with the last"

    everyone = bytes([0x7F, *request[1:]])
    lib.rx_flush(handle)
    lib.rx_bytes(handle, bytes([SYNC, _pid(ID_MASTER_REQ), *everyone, _checksum(everyone)]))
    assert lib.rx_frame(handle)["data"] == everyone, "the broadcast NAD"

    other = bytes([0x11, *request[1:]])
    lib.rx_flush(handle)
    lib.rx_bytes(handle, bytes([SYNC, _pid(ID_MASTER_REQ), *other, _checksum(other)]))
    assert lib.rx_active(handle) is False
    assert lib.rx_index(handle) == 10, "another node's request is dropped at its last byte, before the callback"


def test_ALX1564_P697_a_slave_answers_the_slave_response_header_only_with_a_request_pending(slave):
    """Nothing pending, nothing sent; pending, the response goes out and the flag clears."""
    lib, handle = slave

    lib.rx_flush(handle)
    lib.rx_bytes(handle, bytes([SYNC, _pid(ID_SLAVE_RESP)]))
    assert lib.rx_active(handle) is False
    assert lib.wire(handle) == b""

    lib.set_slave_req_pending(handle, True)
    lib.rx_flush(handle)
    lib.rx_bytes(handle, bytes([SYNC, _pid(ID_SLAVE_RESP)]))
    assert lib.slave_req_pending(handle) is False
    assert lib.wire(handle) == bytes(8) + bytes([_checksum(bytes(8))]), "the weak callback's zeros, classic"
