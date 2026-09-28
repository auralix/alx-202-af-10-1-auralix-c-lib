"""Auralix C Library - AlxFsSafe on the host.

Two copies of every file, each with a CRC after it, and the logic that decides which to believe.
It is what makes a stored setting survive losing power halfway through writing it: the write puts
copy A down and then copy B, so an interruption leaves one of them intact, and the next read
notices, picks the good one and REPAIRS the other on the spot.

The naming is worth knowing before reading anything else: copy A keeps the plain name and copy B
gets a `B` inserted before the extension, so `param.bin` and `paramB.bin` are the pair. Copy A
therefore has the same name as a pre-CRC file would, which is what makes the `useOrig` migration
work in place (P446).

The file system is faked, so a test can put a corrupt copy on the "flash" before the module ever
runs - which is the only way to reach the recovery paths at all - and can make any one operation
fail. The CRC is the module's own, read back through the helper rather than restated, because a
test that computed its own would be checking its own polynomial.

One thing sealed: when a repair cannot be written, the caller is given an error and NO DATA, even
though a good copy was read and validated (P447).

Proofs (ALX-1553): P440-P448.
"""

import pytest

PATH = "param.bin"
DATA = bytes(range(8))
OTHER = bytes([0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7])

# alxGlobal.h, in declaration order from AlxSafe_BothCopyErr_OrigErr.
BOTH_ERR = 10
ORIG_OK_USE_ORIG = 11
BOTH_OK_CRC_SAME = 12
BOTH_OK_CRC_DIFF = 13
A_OK_B_ERR = 14
A_ERR_B_OK = 15

ALX_OK = 0


@pytest.fixture
def safe(fs_safe_lib):
    """Safe storage over an empty flash, with the legacy migration off."""
    lib = fs_safe_lib
    return lib, lib.new()


def test_ALX1553_P440_a_write_puts_two_copies_down_each_with_its_own_crc(safe):
    """One call, two files - the plain name and the B name - and both are data plus CRC.

    The point of the module in one test: after a successful write there are two independent copies
    on the flash, so losing either one loses nothing. Checked as BYTES rather than through a read
    back, because a read back would pass even if both copies were the same file.
    """
    lib, handle = safe

    assert lib.write(handle, PATH, DATA) == ALX_OK

    expected = lib.copy_bytes(handle, DATA)
    assert lib.copy(handle, PATH, "a") == expected
    assert lib.copy(handle, PATH, "b") == expected
    assert lib.has("param.bin"), "copy A is not a file of its own"
    assert lib.has("paramB.bin"), "copy B is not a file of its own"
    assert len(expected) == len(DATA) + lib.crc_len(handle)


def test_ALX1553_P441_what_was_written_reads_back_and_says_both_copies_agreed(safe):
    """The ordinary round trip, and the status that says nothing had to be repaired.

    The status is half the answer here. A safe store that silently repaired on every read would
    also return the right bytes, and the only way to know it did not is that it said so.
    """
    lib, handle = safe
    lib.write(handle, PATH, DATA)

    status, got = lib.read(handle, PATH, len(DATA))

    assert status == BOTH_OK_CRC_SAME
    assert got == DATA


def test_ALX1553_P442_a_corrupt_second_copy_is_noticed_and_rewritten(safe):
    """Copy B fails its CRC: copy A is used, and B is put back from A before the call returns.

    Repairing on READ is what keeps the redundancy from decaying - a copy that went bad and was
    only ever read around would leave the device one fault from losing the data, indefinitely, with
    nothing reporting it.
    """
    lib, handle = safe
    lib.write(handle, PATH, DATA)
    lib.put_raw("paramB.bin", bytes(len(DATA) + lib.crc_len(handle)))   # right length, wrong CRC

    status, got = lib.read(handle, PATH, len(DATA))

    assert status == A_OK_B_ERR
    assert got == DATA
    assert lib.copy(handle, PATH, "b") == lib.copy_bytes(handle, DATA), "copy B was not repaired"


def test_ALX1553_P443_a_missing_first_copy_is_taken_from_the_second(safe):
    """Copy A gone: copy B is used, and A is written back from it. The mirror of P442.

    Both directions are checked because they are two separate branches with two separate repair
    calls, and the one that repairs A is the one a half-finished WRITE leaves behind.
    """
    lib, handle = safe
    lib.write(handle, PATH, DATA)
    lib.put_raw("param.bin", b"")                                       # A truncated to nothing

    status, got = lib.read(handle, PATH, len(DATA))

    assert status == A_ERR_B_OK
    assert got == DATA
    assert lib.copy(handle, PATH, "a") == lib.copy_bytes(handle, DATA), "copy A was not repaired"


def test_ALX1553_P444_with_both_copies_gone_nothing_is_invented(safe):
    """No copies and no original: an error, and the caller's buffer is left as it was.

    The one outcome where the module has nothing to offer. What matters is that it says so rather
    than handing back the zeroed scratch buffer as if it were data - a caller that trusted it would
    write zeros into a configuration.
    """
    lib, handle = safe

    status, _ = lib.read(handle, "never-written.bin", len(DATA))

    assert status == BOTH_ERR


def test_ALX1553_P445_two_good_copies_that_disagree_resolve_to_the_first(safe):
    """Both copies pass their CRC and hold different data - A wins, and B is made to match.

    This is the state a power loss between the two writes leaves behind: A is the new value, B is
    still the old one, and both are perfectly valid. A is chosen because A is written first, so it
    is the newer of the two - which is only true because of the write ORDER, and that is what makes
    P448 worth having next to this.
    """
    lib, handle = safe
    lib.put_copy(handle, PATH, "a", DATA)
    lib.put_copy(handle, PATH, "b", OTHER)

    status, got = lib.read(handle, PATH, len(DATA))

    assert status == BOTH_OK_CRC_DIFF
    assert got == DATA, "the older copy won"
    assert lib.copy(handle, PATH, "b") == lib.copy_bytes(handle, DATA), "copy B was not updated"


def test_ALX1553_P446_a_file_from_before_the_crc_is_migrated_in_place(fs_safe_lib):
    """CHARACTERIZATION: with useOrig on, a plain file with no CRC is read once and then replaced.

    The migration path, and it works because of the naming: copy A has the SAME name as the
    original, so writing copy A overwrites the file that was just read, and copy B appears beside
    it. After one read the device is on the new format and the old file no longer exists as such.

    The status says which path was taken, which is the only signal a product gets that a migration
    happened at all - there is no separate call and no flag.
    """
    lib = fs_safe_lib
    handle = lib.new(use_orig=True)
    lib.put_raw(PATH, DATA)                                     # the old format: data, no CRC

    status, got = lib.read(handle, PATH, len(DATA))

    assert status == ORIG_OK_USE_ORIG
    assert got == DATA
    assert lib.copy(handle, PATH, "a") == lib.copy_bytes(handle, DATA), "A did not replace it"
    assert lib.copy(handle, PATH, "b") == lib.copy_bytes(handle, DATA), "B was not created"

    assert lib.read(handle, PATH, len(DATA)) == (BOTH_OK_CRC_SAME, DATA), "it migrated twice"


@pytest.mark.xfail(strict=True, reason=(
    "DEFECT: when AlxFsSafe_File_Read cannot WRITE the repair, it returns that write's status and "
    "never copies the data it already has. Every recovery branch repairs first and memcpy()s "
    "second, so a good copy that was read, CRC-checked and is sitting in the module's own buffer "
    "is thrown away because the OTHER copy could not be refreshed. The caller gets an error and an "
    "untouched buffer. A flash that has filled up or worn out still READS, so this turns a device "
    "that could have kept running on valid stored settings into one that cannot read them at all - "
    "and it is the repair, an optimisation for the next boot, that costs it. The data should be "
    "delivered first and the repair failure reported alongside it, not instead of it"))
def test_ALX1553_P447_a_repair_that_cannot_be_written_still_returns_the_good_copy(safe):
    """A good copy was read. A failing repair must not take it away from the caller."""
    lib, handle = safe
    lib.write(handle, PATH, DATA)
    lib.put_raw("paramB.bin", bytes(len(DATA) + lib.crc_len(handle)))   # B corrupt, A fine

    lib.fail("write", -1)                                              # the flash refuses writes
    status, got = lib.read(handle, PATH, len(DATA))

    assert got == DATA, "a validated copy was discarded because the repair failed"
    assert status in (A_OK_B_ERR, ALX_OK)


def test_ALX1553_P448_a_write_interrupted_after_the_first_copy_is_recovered_by_the_next_read(safe):
    """CHARACTERIZATION: the whole point of the module, end to end.

    A write puts copy A down and then copy B. Failing the SECOND write is what losing power between
    them looks like: the write reports the error, so a caller is never told the new value is safe,
    but copy A already holds it. The next read then finds one good copy and one it cannot use,
    takes the good one and repairs the other - and the device comes up on the NEW value, with the
    redundancy restored.

    Delivering the new value is a choice rather than a law. The interrupted write had not finished,
    so committing to it is as defensible as rolling back, and neither is written down anywhere;
    this test is what says which one it does.

    What the state of copy B is in between is deliberately NOT asserted. This fake truncates when a
    file is opened for writing, so B reads as empty; littlefs does not commit anything until the
    file is closed, so on a device the old B would very likely still be there. Either way the copy
    is unusable or stale and the next read repairs it, which is the part that has to hold.
    """
    lib, handle = safe
    lib.write(handle, PATH, DATA)

    lib.fail("write", 1, after=1)                             # copy A lands, copy B does not
    assert lib.write(handle, PATH, OTHER) != ALX_OK, "a half-finished write reported success"

    assert lib.copy(handle, PATH, "a") == lib.copy_bytes(handle, OTHER), "copy A is not the new value"
    assert lib.copy(handle, PATH, "b") != lib.copy_bytes(handle, OTHER), "copy B was written after all"

    status, got = lib.read(handle, PATH, len(DATA))

    assert status in (A_OK_B_ERR, BOTH_OK_CRC_DIFF), "the interrupted write was not noticed"
    assert got == OTHER, "the interrupted write was rolled back rather than completed"
    assert lib.copy(handle, PATH, "b") == lib.copy_bytes(handle, OTHER), "the redundancy was not restored"
    assert lib.read(handle, PATH, len(DATA)) == (BOTH_OK_CRC_SAME, OTHER), "and it stays repaired"
