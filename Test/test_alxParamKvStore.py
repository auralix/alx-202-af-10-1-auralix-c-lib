"""Auralix C Library - AlxParamKvStore on the host.

The store every named, persisted parameter on a device goes through: a key is a file name and a
value is that file's bytes. The module itself is thirty lines of forwarding to `AlxFs`, which makes
it look like there is nothing to test - until you ask what it does when the file system says no.

That is what this group is about, and it is why the file system is faked rather than linked: the
fake can be told to fail any one operation, once or always, so the error paths are REACHABLE. A
module whose error handling is wrong passes every test that never makes anything fail.

The fake also counts opens against closes, which is the only way to see a leaked file handle from
outside.

Two things a reader should know first, both sealed: a failed read or write leaves the file OPEN
(P433), and a single failed mount FORMATS the flash - destroying every stored parameter - and
reports success (P434).

Proofs (ALX-1553): P431-P439.
"""

import pytest

KEY = "MY_PARAM"
VALUE = bytes([0x01, 0x02, 0x03, 0x04])
OTHER = bytes([0xAA, 0xBB])

ALX_OK = 0


@pytest.fixture
def store(param_kv_store_lib):
    """An initialised store over a healthy, empty file system."""
    lib = param_kv_store_lib
    return lib, lib.new()


def test_ALX1553_P431_a_value_written_comes_back_and_reaches_the_file_system(store):
    """The round trip, and the file underneath it.

    Both halves matter. A store that kept values in RAM and never wrote them would pass the round
    trip and lose everything on the next boot, so the test also looks at what is on the "flash"
    under the key the caller used - which is the thing a later boot will read.
    """
    lib, handle = store

    assert lib.set(handle, KEY, VALUE) == ALX_OK
    assert lib.on_flash(KEY) == VALUE, "the value never reached the file system"

    status, got = lib.get(handle, KEY)

    assert status == ALX_OK
    assert got == VALUE


def test_ALX1553_P432_a_healthy_operation_closes_the_file_it_opened(store):
    """Every Get and every Set opens exactly one file and closes it again.

    The baseline for P433: on the path where nothing fails, the count is exact, so a difference
    afterwards is the error path and not a miscount.
    """
    lib, handle = store

    lib.set(handle, KEY, VALUE)
    lib.get(handle, KEY)
    lib.set(handle, KEY, OTHER)
    lib.get(handle, KEY)

    assert lib.opens() == 4
    assert lib.closes() == lib.opens(), "a file was opened and not closed"


@pytest.mark.xfail(strict=True, reason=(
    "DEFECT: AlxParamKvStore_Get and _Set return the moment the read or the write fails, without "
    "closing the file they opened. Every function in the module has the same shape - open, do one "
    "thing, close - and in both of them the middle step returns early past the close. On littlefs "
    "an open file holds a cache block and an entry in the mount's file list, and the data is not "
    "synced until it is closed, so a device that hits a read error on a parameter leaks a handle "
    "every time it tries; nothing releases it short of unmounting. The fix is the usual one for "
    "this shape: close first, keep the earlier status, and return the worse of the two"))
@pytest.mark.parametrize("failing", ["read", "write"])
def test_ALX1553_P433_a_failed_transfer_still_closes_the_file(store, failing):
    """A file opened on the way to an error is a file that still has to be closed."""
    lib, handle = store
    lib.set(handle, KEY, VALUE)
    opens_before, closes_before = lib.opens(), lib.closes()

    lib.fail(failing, 1)
    if failing == "read":
        status, _ = lib.get(handle, KEY)
    else:
        status = lib.set(handle, KEY, OTHER)

    assert status != ALX_OK, "the injected failure did not reach the module"
    assert lib.opens() == opens_before + 1, "the module did not open a file at all"
    assert lib.closes() == closes_before + 1, f"a failed {failing} left the file open"


@pytest.mark.xfail(strict=True, reason=(
    "DEFECT: AlxParamKvStore_Init answers a failed mount by FORMATTING. One failed AlxFs_Mount - a "
    "transient read error, a superblock half-written when the power went, a card seated badly - "
    "and the next line erases the whole file system, mounts the empty result and returns Alx_Ok. "
    "Every stored parameter on the device is gone, replaced by defaults, and the caller is told "
    "the store initialised normally. There is no second mount attempt, no trace above INF, and no "
    "status a caller could act on. Formatting may well be the right LAST resort, but it should be "
    "the last one: retry the mount, and when the format does happen, say so in the status so a "
    "product can log it, refuse to run, or re-seed from a backup copy"))
def test_ALX1553_P434_a_failed_mount_does_not_destroy_what_was_stored(param_kv_store_lib):
    """A store that cannot be mounted must not erase itself on the first try."""
    lib = param_kv_store_lib
    handle = lib.new(init=False)
    lib.put_on_flash("KEEP_ME", VALUE)

    lib.fail("mount", 1)
    lib.init(handle)

    assert lib.has_on_flash("KEEP_ME"), "one failed mount erased every stored parameter"
    assert lib.formats() == 0


# =====================================================================
# P435-P439 - what the rest of the module does
# =====================================================================


def test_ALX1553_P435_reading_a_key_that_was_never_written_fails_and_leaves_the_store_usable(store):
    """An absent key is an error, not an empty value - and it does not poison the store.

    The distinction is the whole reason a store returns a status: a parameter that has never been
    written and one written as zero are different things, and a caller that cannot tell them apart
    will seed defaults over real settings.
    """
    lib, handle = store

    status, got = lib.get(handle, "NEVER_WRITTEN")

    assert status != ALX_OK
    assert got == b""
    assert lib.is_init(handle) is True, "a missing key de-initialised the store"
    assert lib.set(handle, KEY, VALUE) == ALX_OK, "the store stopped working after a missing key"


def test_ALX1553_P436_a_removed_key_is_gone_and_removing_it_again_is_an_error(store):
    """Remove takes the key off the file system, and says so when there was nothing to take."""
    lib, handle = store
    lib.set(handle, KEY, VALUE)
    assert lib.has_on_flash(KEY)

    assert lib.remove(handle, KEY) == ALX_OK
    assert not lib.has_on_flash(KEY)
    assert lib.get(handle, KEY)[0] != ALX_OK

    assert lib.remove(handle, KEY) != ALX_OK, "removing an absent key reported success"


def test_ALX1553_P437_writing_a_value_replaces_the_old_one_rather_than_adding_to_it(store):
    """A shorter value leaves no tail of the longer one it replaced.

    Which is a property of the MODE the module asks for - "w", which truncates - and that is what
    is checked, rather than trusting the fake to model truncation. A store that opened for append
    would grow a parameter on every write and read back the first value it ever held.
    """
    lib, handle = store

    lib.set(handle, KEY, VALUE)
    assert lib.last_open_mode() == "w", "a write opened the file for something other than writing"

    lib.set(handle, KEY, OTHER)

    assert lib.last_open_mode() == "w"
    assert lib.on_flash(KEY) == OTHER, "the shorter value left a tail of the longer one"
    assert lib.get(handle, KEY) == (ALX_OK, OTHER)
    assert lib.last_open_mode() == "r", "a read opened the file for something other than reading"


@pytest.mark.parametrize(("failing", "expected_formats"), [
    ("format", 1),      # the mount failed, the format was tried and failed too
    ("mount", 1),       # the mount failed, the format worked, the second mount failed
])
def test_ALX1553_P438_an_init_that_cannot_recover_reports_it_and_stays_uninitialised(
    param_kv_store_lib, failing, expected_formats,
):
    """CHARACTERIZATION: when Init really cannot go on, it returns the error and isInit stays false.

    Both ways out of the recovery path. This is the behaviour P434 does NOT have: here the module
    is honest, because there is nothing left to try. The contrast is the point - the silent case is
    the one where recovery appeared to work.
    """
    lib = param_kv_store_lib
    handle = lib.new(init=False)

    lib.fail("mount", 2 if failing == "mount" else 1)
    if failing == "format":
        lib.fail("format", 1)

    status = lib.init(handle)

    assert status != ALX_OK
    assert lib.is_init(handle) is False, "a store that failed to initialise says it did"
    assert lib.formats() == expected_formats


def test_ALX1553_P439_deinit_unmounts_and_a_failed_unmount_leaves_the_store_initialised(store):
    """DeInit unmounts and clears the flag - and when the unmount fails, neither happens.

    Keeping isInit true on a failed unmount is the safe way round: the object still matches the
    file system's actual state, so a caller can try again. A DeInit that cleared the flag anyway
    would leave a mounted file system nothing owns.
    """
    lib, handle = store
    assert lib.mounted() is True

    lib.fail("unmount", 1)
    assert lib.deinit(handle) != ALX_OK
    assert lib.is_init(handle) is True, "a failed unmount de-initialised the store anyway"
    assert lib.mounted() is True

    assert lib.deinit(handle) == ALX_OK
    assert lib.is_init(handle) is False
    assert lib.mounted() is False
