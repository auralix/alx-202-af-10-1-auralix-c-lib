"""Auralix C Library - AlxId on the host.

The identity block is what every Auralix product prints when it comes up: the firmware's artifact
and name, its version, the date and the git hash it was built from, the bootloader's own copy of
all of that read back out of flash, and which board the firmware finds itself running on. Nothing
downstream re-derives any of it - the Python library parses this block out of a boot trace and the
device suite uses it to say WHICH IMAGE a bench run actually ran against. A wrong identity block
does not break a board; it makes every result describe the wrong binary. Until this file, nothing
tested it.

Three things are worth knowing before reading any test here.

The object has TWO HALVES WITH OPPOSITE LIFECYCLES. Everything about the firmware - the version,
the packed 64-bit word, both rendered strings, the whole bootloader block - is finished by the
CONSTRUCTOR; ``AlxId_Init`` contributes nothing to it. The board half is the reverse and exists
only after Init. Every getter asserts ``isInit`` regardless, and on a product with asserts enabled
that assertion is a reset (P472).

The objects are POISONED WITH 0xFF before the constructor runs, which is the house trick and here
it is load-bearing rather than decorative: the central finding of this file (P482) is invisible on
a zero-filled object, and P483 is the test that says so.

AlxId_Trace's TEXT IS NOT TESTED and cannot be from this group. ``Test/alxConfig.h`` sets
``ALX_TRACE_LEVEL_OFF``, which compiles ``ALX_TRACE_INF`` itself away, so the module's whole trace
paragraph reduces to its two assertions - which is what P504 pins. Proving the wire shape the
Python parser keys on ("FW:", "- ver:", "- bin:", the INF framing) would mean rewiring alxConfig.h
and linking the real alxTrace.c into this group; the getters those lines are formatted from are
pinned here instead (P473, P484).

Sealed defects: P475, P476, P481, P482, P486, P487, P489, P490, P494, P496.

Proofs (ALX-1553): P468-P505.
"""

import ctypes

import pytest

# ALX_ID_NAME_LEN, ALX_ID_FW_VER_STR_LEN and ALX_ID_FW_BIN_STR_LEN of alxId.h. Restated here rather
# than exported, because these three are the shape of the PUBLIC type - a test that read them from
# the module could not notice the module changing them, which is most of what P475 and P476 are for.
NAME_LEN = 40
VER_STR_LEN = 96
BIN_STR_LEN = 96

# A blob a bootloader could plausibly have written: a version, a date, a hash, nothing exotic.
BOOT_HASH = "d" * 40
BOOT = {
    "artf": "BOOT-ARTF",
    "name": "BOOT-NAME",
    "fw_ver": (4, 5, 6),
    "build_date_comp": 2401021200,
    "build_date": 2409091111,
    "hash_": BOOT_HASH,
    "hash_short": "deadbee",
    "hash_short_uint32": 0x0DEADBEE,
}

_RAW_GETTER = ctypes.CFUNCTYPE(ctypes.c_void_p, ctypes.c_void_p)


def _field_bytes(lib, obj, getter, length):
    """The raw bytes one string getter points at - terminator or not.

    ``IdLib.text`` reads through ``c_char_p``, which stops at the first zero byte and so cannot
    tell a field that ends inside itself from one that ran off its end into the next member. This
    takes the pointer instead and reads exactly the field's own width.
    """
    return ctypes.string_at(_RAW_GETTER((f"AlxId_Get{getter}", lib.c))(obj), length)


def _packed(major, minor, patch, date):
    """The 64-bit word alxId.c composes - written out here so a test states the layout it expects."""
    return (major << 56) | (minor << 48) | (patch << 32) | date


def _fw(lib, **ctor):
    """An identity with one hardware variant and no straps, constructed and initialised."""
    obj = _fw_ctor(lib, **ctor)
    lib.init(obj)
    return obj


def _fw_ctor(lib, *, poison=0xFF, hw_id=1, pcb_ver=(0, 0, 0), pcb_date=0, **ctor):
    """The same, stopped after the constructor - for the tests that look before Init."""
    obj = lib.alloc(poison)
    lib.set_pcb(lib.instance(obj), hw_id=hw_id, ver=pcb_ver, date=pcb_date)
    lib.set_bom(lib.instance(obj))
    lib.ctor_no_hw_id(obj, **ctor)
    return obj


def _board(lib, straps, known, supported, **ctor):
    """An identity that reads its board off the straps: ``known`` is a list of (id, name)."""
    obj = lib.alloc()
    for i, (hw_id, name) in enumerate(known):
        lib.set_pcb(lib.known(obj, i), hw_id=hw_id, name=name)
        lib.set_bom(lib.known(obj, i))
    for i, hw_id in enumerate(supported):
        lib.set_supported(obj, i, hw_id)
    lib.ctor(obj, known_len=len(known), supported_len=len(supported), pin_len=len(straps), **ctor)
    for i, state in enumerate(straps):
        lib.strap(obj, i, state)
    return obj


# =====================================================================
# The firmware version: one 64-bit word, composed once, in the constructor
# =====================================================================


def test_ALX1553_P468_an_identity_block_carries_the_firmware_it_was_constructed_with(id_lib):
    """The version handed to the constructor is what the getters answer after Init.

    The smallest claim this group can make, and the one every other test in it rests on: the
    object can be built at all, the no-hardware-id constructor is enough to reach Init, and a
    field put in one end comes out the other. The object is poisoned with 0xFF before the
    constructor runs, so a major version reading 7 is the constructor having written it and not a
    fresh allocation happening to be zero.
    """
    obj = id_lib.alloc()
    id_lib.set_pcb(id_lib.instance(obj), hw_id=1, ver=(1, 0, 0), date=2601010000)
    id_lib.set_bom(id_lib.instance(obj), ver=(1, 0, 0), date=2601010000)
    id_lib.ctor_no_hw_id(obj, artf="ALX-1-AF-1", name="DemoApp", ver=(7, 8, 9))
    id_lib.init(obj)

    assert id_lib.is_init(obj)
    assert id_lib.text(obj, "FwArtf") == "ALX-1-AF-1"
    assert id_lib.text(obj, "FwName") == "DemoApp"
    assert (id_lib.num(obj, "FwVerMajor"),
            id_lib.num(obj, "FwVerMinor"),
            id_lib.num(obj, "FwVerPatch")) == (7, 8, 9)


def test_ALX1553_P469_the_packed_version_leaves_an_eight_bit_hole_between_minor_and_patch(id_lib):
    """The word is major<<56 | minor<<48 | patch<<32 | verDate, so bits 40..47 can never be set.

    This is the machine-readable identity the whole verification chain compares against, and it is
    NOT the layout the header's field order suggests. Reading major, minor, patch, verDate as four
    adjacent fields gives patch<<40 - a byte that is always zero, so a decoder written that way
    reports patch 0 for every build ever made and never fails. The hole exists because verDate is a
    full uint32 occupying bits 0..31 while the three version bytes are packed from the top down,
    and nothing in the header says so.

    Driven at the extreme: all three version bytes 255 and the date all ones, which sets every bit
    the composition can reach. The 00 byte in the middle of the answer is the entire point.
    """
    obj = _fw(id_lib, ver=(255, 255, 255), date_comp=0xFFFFFFFF)

    ver = id_lib.num(obj, "FwVer")

    assert ver == 0xFFFF00FFFFFFFFFF, "the packed version reached a bit the layout cannot address"
    assert (ver >> 40) & 0xFF == 0, "bits 40..47 are not addressable by any input"


@pytest.mark.parametrize(("ver", "date"), [
    ((1, 0, 0), 0),
    ((0, 1, 0), 0),
    ((0, 0, 1), 0),
    ((0, 0, 0), 1),
    ((0, 0, 0), 0xFFFFFFFF),
    ((255, 255, 255), 0xFFFFFFFF),
    ((1, 2, 3), 2401021200),
    ((255, 0, 255), 2609102334),
])
def test_ALX1553_P470_no_version_field_can_reach_into_another_fields_slot(id_lib, ver, date):
    """Each source is narrower than the slot it is shifted into, so the composition is reversible.

    The interesting part is the margin, which is exactly zero: verDate fills bits 0..31 completely
    and the three version bytes fill their own bytes completely. Widening verDate, or moving patch
    to the <<40 the field order implies, starts aliasing on the very first build - two different
    firmware images reporting one identity, with nothing anywhere to notice. Pinning the decomposition
    is what makes that a test failure rather than a bench result attributed to the wrong binary.

    The boundary values are swept rather than one example, because aliasing shows up at the edges of
    a field and nowhere else.
    """
    obj = _fw(id_lib, ver=ver, date_comp=date)

    word = id_lib.num(obj, "FwVer")

    assert word == _packed(ver[0], ver[1], ver[2], date)
    assert ((word >> 56) & 0xFF, (word >> 48) & 0xFF, (word >> 32) & 0xFF) == ver
    assert word & 0xFFFFFFFF == date


def test_ALX1553_P471_the_build_job_flag_chooses_which_date_becomes_the_version_date(id_lib):
    """isBuildJobUsed true takes ALX_BUILD_DATE; false takes the constructor's fwBuildDateComp.

    One flag decides whether the identity says when the CI job ran or when the compiler ran, and
    verDate is not a leaf: it is the low half of the packed version, the fourth field of verStr and
    the FIRST field of the .bin filename. Inverted, a locally built image and a job-built image swap
    their dates and the bench attributes a run to the wrong binary - which is the exact failure this
    module exists to prevent, and the two dates are close enough to look plausible either way.

    The compile-stamp value driven here is deliberately not a date at all, so neither branch can be
    mistaken for the other. The build date is read back from the macro rather than written down:
    alxBuild.py rewrites alxBuild_GENERATED.h on every firmware build.
    """
    not_a_date = 1111111111
    build_date = id_lib.build("Date")
    assert build_date != not_a_date, "the two dates under test have to be distinguishable"

    off = _fw(id_lib, is_build_job_used=False, date_comp=not_a_date)
    on = _fw(id_lib, is_build_job_used=True, date_comp=not_a_date)

    assert id_lib.num(off, "FwVerDate") == not_a_date
    assert id_lib.num(on, "FwVerDate") == build_date
    assert id_lib.num(off, "FwVer") & 0xFFFFFFFF == not_a_date
    assert id_lib.num(on, "FwVer") & 0xFFFFFFFF == build_date


@pytest.mark.expect_assert(
    "reading the firmware version before Init is the whole subject of P472 - every getter asserts "
    "isInit even when the value it returns was finished by the constructor"
)
def test_ALX1553_P472_the_firmware_half_is_finished_by_the_ctor_and_the_board_half_only_by_init(id_lib):
    """Two halves of one object, opposite lifecycles, and one lifecycle rule over both.

    The firmware version, its string and the .bin filename are complete the moment the constructor
    returns - Init recomputes none of them, and reading them before Init returns exactly what reading
    them after returns. The board half is the mirror image: pcbVer is still whatever was in memory
    until Init composes it. Yet every getter asserts isInit, so a caller reading the firmware version
    during early boot - the natural thing to want, since the value is sitting there correct - trips an
    assertion that on a product with ALX_ID_ASSERT_RST_ENABLE is a RESET.

    Four assertions are counted, one per pre-Init getter, which is also the statement that the
    assertion is per call and not once per object. Recorded text: "RST alxId.h:756 in AlxId_GetFwVer"
    (ALX_ID_FILE is the header's name while the line number is alxId.c's).

    This is the test that has to fail if someone ever moves the version composition into Init to
    "tidy up" the constructor - which would break every caller that reads it early while looking,
    from the outside, like a refactor.
    """
    obj = _fw_ctor(id_lib, ver=(1, 2, 3), date_comp=2401021200)

    before = (id_lib.num(obj, "FwVer"), id_lib.text(obj, "FwVerStr"), id_lib.text(obj, "FwBinStr"))
    pcb_before = id_lib.num(obj, "HwPcbVer")
    tripped = id_lib.c.AlxAssertPc_Count()

    id_lib.init(obj)

    assert tripped == 4, "the four pre-Init reads did not each record their own assertion"
    assert "AlxId_GetFwVer" in (id_lib.c.AlxAssertPc_First() or b"").decode("ascii", "replace")
    assert before == (id_lib.num(obj, "FwVer"), id_lib.text(obj, "FwVerStr"),
                      id_lib.text(obj, "FwBinStr")), "Init changed the firmware version surface"
    assert pcb_before == 0xFFFFFFFFFFFFFFFF, "the board version was composed before Init"
    assert id_lib.num(obj, "HwPcbVer") == 0, "Init did not compose the board version"


# =====================================================================
# The two rendered strings: the contract with everything outside the device
# =====================================================================


@pytest.mark.parametrize(("ver", "date"), [
    ((1, 2, 3), 2401021200),
    ((255, 255, 255), 2412312359),
    ((0, 0, 0), 0),
])
def test_ALX1553_P473_the_version_string_and_the_bin_filename_have_a_fixed_shape(id_lib, ver, date):
    """verStr is "major.minor.patch.date.fullHash"; binStr is "date_artf_name_Vmajor-minor-patch_shortHash.bin".

    These two strings are the wire format with everything outside the device. alx.c_lib.trace's
    identity parser reads them out of the boot trace, and alxBin.py independently builds the very
    same filename on the host to name the artifact it just built - so the two have to agree
    character for character or a bench run cannot be matched to its image. A changed separator or a
    reordered field breaks a parser three repositories away rather than the board, and nothing on
    the device would report anything wrong.

    Worth noting which hash goes where: verStr carries the FULL 40-character hash and binStr the
    7-character short one, and there is no getter for the long one at all - parsing it back out of
    verStr is the only way to reach it, which is exactly what the Python side has to do.

    Three versions, because "255" shows the fields are not zero padded and "0.0.0" shows they are not
    suppressed.
    """
    major, minor, patch = ver
    obj = _fw(id_lib, artf="ARTF", name="NAME", ver=ver, date_comp=date)

    assert id_lib.text(obj, "FwVerStr") == f"{major}.{minor}.{patch}.{date}.{id_lib.build('Hash')}"
    assert id_lib.text(obj, "FwBinStr") == (
        f"{date}_ARTF_NAME_V{major}-{minor}-{patch}_{id_lib.build('HashShort')}.bin")


def test_ALX1553_P474_the_version_string_fits_its_field_in_the_worst_case_the_inputs_allow(id_lib):
    """verStr is an unbounded sprintf into 96 bytes that is safe by construction, and binStr is not.

    Worth stating as its own fact because the two strings sit next to each other, are built the same
    way from the same ingredients, and only one of them is safe. verStr's inputs are all bounded:
    three version bytes at most 3 characters each, a uint32 date at most 10, a hash the generator
    fixes at 40, three separator dots. That is 63 of 96 at the absolute maximum, measured here at
    the maximum rather than argued. binStr interpolates two 40-byte NAME fields instead, which is
    where the 96 goes (P475).
    """
    obj = _fw(id_lib, ver=(255, 255, 255), date_comp=0xFFFFFFFF)

    rendered = id_lib.text(obj, "FwVerStr")

    assert rendered == f"255.255.255.4294967295.{id_lib.build('Hash')}"
    assert len(rendered) == 63
    assert b"\x00" in _field_bytes(id_lib, obj, "FwVerStr", VER_STR_LEN), "verStr did not terminate"


@pytest.mark.xfail(strict=True, reason=(
    "DEFECT: AlxId_Ctor renders the .bin filename with an unbounded sprintf into "
    "me->fw.binStr, which is char[ALX_ID_FW_BIN_STR_LEN] = 96. The format "
    "\"%lu_%s_%s_V%u-%u-%u_%s.bin\" costs 37 bytes at the widest fixed inputs (10 digit date, "
    "three 3-digit version fields, 7 character short hash, six separators and \".bin\") and then "
    "interpolates fwArtf and fwName, each of which ALX_ID_NAME_LEN permits to be 39 characters. "
    "Both fields full is 37 + 78 + NUL = 116 bytes into 96, so the write runs 20 bytes past the "
    "end of binStr - the LAST member of the packed AlxId_Fw - into me->fwIsBootUsed and "
    "me->fwBootIdAddr. Measured: the artifact and name at the capacity the header allows render "
    "115 characters; the overflow starts once artf + name reaches 59. The constructor rewrites "
    "fwIsBootUsed on the next line, which plants a zero byte and makes the getter return exactly "
    "96 characters instead of crashing - so the visible symptom is a silently WRONG .bin filename, "
    "which is the one string the bench uses to say which image it ran. ASan cannot see it either: "
    "it is an intra-object overflow inside one allocation. The fix is snprintf with sizeof the "
    "field, or a length check on the two names the constructor already copies unchecked (P476)"))
def test_ALX1553_P475_the_bin_filename_fits_its_field_for_names_the_header_allows(id_lib):
    """The filename should terminate inside its own 96 bytes for any artifact and name ALX_ID_NAME_LEN permits.

    39 characters each is not an adversarial input - it is exactly what the two char[40] fields are
    sized to hold, and two ordinary product names ("ALX-1-AF-1-1-MainApplication" and its artifact,
    say) already reach the 59 where this starts.
    """
    obj = _fw(id_lib, artf="A" * (NAME_LEN - 1), name="N" * (NAME_LEN - 1),
              ver=(255, 255, 255), date_comp=2412312359)

    assert b"\x00" in _field_bytes(id_lib, obj, "FwBinStr", BIN_STR_LEN), (
        "the .bin filename ran off the end of its own field")


@pytest.mark.xfail(strict=True, reason=(
    "DEFECT: AlxId_Ctor copies its two name parameters with plain strcpy - strcpy(me->fw.artf, "
    "fwArtf) and strcpy(me->fw.name, fwName) at alxId.c:99-100 - into char[ALX_ID_NAME_LEN] "
    "fields, with no length check anywhere and no way for a caller to learn the limit from the "
    "header, which declares the parameters as bare const char*. A 60 character artifact leaves "
    "artf with no terminator inside its own 40 bytes and 21 bytes of it written over the head of "
    "name; the next strcpy then writes name from its own offset 0, so AlxId_GetFwArtf returns a 44 "
    "character string - the 40 of artf plus the 4 of the name that followed it. Measured exactly "
    "that. The getter's return type gives the caller no length either, so nothing downstream can "
    "detect it. This is the same class as P475 one step earlier, and it is the first thing the "
    "constructor does with caller data. The fix is a bounded copy plus an ALX_ID_ASSERT on the "
    "length, the way the module asserts freely on far smaller things"))
def test_ALX1553_P476_an_over_long_artifact_name_stays_inside_its_own_field(id_lib):
    """The artifact the constructor stores should never be longer than the field declared to hold it."""
    obj = _fw(id_lib, artf="A" * 60, name="NAME")

    assert len(id_lib.text(obj, "FwArtf")) < NAME_LEN, "the artifact name ran past artf into name"


# =====================================================================
# What the build stamped in, and what the caller passed
# =====================================================================


def test_ALX1553_P477_the_short_hash_is_the_builds_and_not_the_objects(id_lib):
    """Every AlxId in an image reports the same commit, whatever it was constructed with.

    AlxId_GetFwHashShort is the only machine-readable link from a running board back to a git
    object - a device reporting which commit it was built from over a numeric channel. Two things
    are pinned. It is the build macro rather than anything recomputed per object, shown by two
    identities built from different arguments answering the same number while their versions
    differ; and the value can never exceed 0x0FFFFFFF, because alxBuild.py derives it from exactly
    7 hex digits, so a consumer treating it as a full 32-bit identifier is reading a quarter of a
    range that does not exist.

    The last assertion is the cross-check the Python side depends on: the numeric hash and the text
    hash are generated from the same sha and must agree, even though alxId.c never derives one from
    the other and nothing would notice if they drifted.
    """
    one = _fw(id_lib, artf="One", name="First", ver=(1, 2, 3), date_comp=2401021200)
    two = _fw(id_lib, artf="Two", name="Second", ver=(9, 8, 7), date_comp=2512250000)

    short = id_lib.num(one, "FwHashShort")

    assert short == id_lib.num(two, "FwHashShort") == id_lib.build("HashShortUint32")
    assert id_lib.num(one, "FwVer") != id_lib.num(two, "FwVer"), "the version is not per object"
    assert short < 0x10000000, "the short hash used a nibble seven hex digits cannot reach"
    assert short == int(id_lib.build("HashShort"), 16), "the numeric and text short hashes disagree"


def test_ALX1553_P478_the_hash_fields_are_full_to_the_byte(id_lib):
    """CHARACTERIZATION: 40 characters in a 48-byte field, and 7 in an 8-byte one - no spare at all.

    alxBuild.py writes sha[0:7] into a field that AlxId_FW_BUILD_HASH_SHORT_LEN sizes at 8, so today
    the 7 characters and their terminator fit with nothing left over, and the constructor's strcpy at
    alxId.c:94 is safe by exactly one byte. The hazard is concrete rather than theoretical: git
    lengthens its abbreviated hashes as a repository grows, and a generator changed to use
    `git rev-parse --short` instead of a fixed slice would write 9 bytes into 8 - the ninth landing
    on the low byte of hashShortUint32, corrupting the numeric commit id with the last character of
    the text one. A test that states the margin is zero is what makes that visible the day it happens.

    The full hash has room (40 of 48) and is the one the generator would not change.
    """
    assert len(id_lib.build("HashShort")) == 7, "the short hash is no longer exactly seven digits"
    assert len(id_lib.build("Hash")) == 40, "the full hash is no longer a full sha"

    obj = _fw(id_lib, artf="ARTF", name="NAME", ver=(1, 0, 0), date_comp=2401021200)

    assert id_lib.text(obj, "FwBinStr").endswith(f"_{id_lib.build('HashShort')}.bin")
    assert id_lib.text(obj, "FwVerStr").endswith(f".{id_lib.build('Hash')}")


@pytest.mark.parametrize("date", [0, 1, 2401021200, 4294967295])
def test_ALX1553_P479_the_version_date_is_passed_through_without_being_looked_at(id_lib, date):
    """CHARACTERIZATION: YYMMDDHHmm packed into a uint32, unvalidated, and the encoding ends in 2043.

    The format is a decimal date squeezed into 32 bits, and it runs out on 1 January 2043 -
    4301010000 does not fit in a uint32. alxId validates nothing and clamps nothing, so whatever the
    caller or the generator produces is what the identity block reports, and a wrapped or garbage
    date propagates unchallenged into the packed version, into verStr and into the .bin filename.

    That is the caller's value to get right and not a defect in this module, but it is worth writing
    down that the module has no opinion at all - and worth having the 2043 ceiling on the record for
    whoever meets it.
    """
    obj = _fw(id_lib, date_comp=date)

    assert id_lib.num(obj, "FwVerDate") == date
    assert id_lib.num(obj, "FwVer") & 0xFFFFFFFF == date
    assert id_lib.text(obj, "FwVerStr").split(".")[3] == str(date)


def test_ALX1553_P480_the_compile_stamp_is_the_compilers_own_clock_and_reaches_the_version_date(id_lib):
    """ALX_BUILD_DATE_COMP is YYMMDDHHmm assembled from __DATE__ and __TIME__, and lands in verDate unchanged.

    This is the value most products actually hand the constructor as fwBuildDateComp: a build that
    did not run through a CI job has no ALX_BUILD_DATE worth having, so the compiler's own clock is
    the date the identity reports. Recomputed here from the same two strings the macro reads, in
    Python's exact arithmetic - which also checks the macro's day rule, since __DATE__ pads a
    single-digit day with a SPACE and ALX_BUILD_COMP_DAY_CH0 maps anything below '0' to '0'.

    What this test does NOT say is that the macro computes it correctly. It computes the right answer
    by accident; P481 is why.
    """
    months = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"]
    comp_date, comp_time = id_lib.comp("Date"), id_lib.comp("Time")
    expected = int(
        comp_date[9:11]                                   # year, last two digits
        + f"{months.index(comp_date[0:3]) + 1:02d}"       # month, as two digits
        + comp_date[4:6].replace(" ", "0")                # day, the macro's space-to-zero rule
        + comp_time[0:2] + comp_time[3:5]                 # hour and minute
    )

    assert id_lib.build("DateComp") == expected, f"{comp_date!r} {comp_time!r} did not stamp as {expected}"

    obj = _fw(id_lib, date_comp=id_lib.build("DateComp"))

    assert id_lib.num(obj, "FwVerDate") == expected
    assert id_lib.text(obj, "FwBinStr").startswith(f"{expected}_")


@pytest.mark.xfail(strict=True, reason=(
    "DEFECT: ALX_BUILD_DATE_COMP (alxBuild.h:102-111) assembles YYMMDDHHmm as "
    "(YEAR_CH2-'0')*1000000000 + (YEAR_CH3-'0')*100000000 + ... in PLAIN INT ARITHMETIC. INT_MAX is "
    "2147483647, so the first term alone overflows for any build year whose last two digits are 22 "
    "or more - every build since January 2022 - and the expression is signed integer overflow, "
    "which is undefined behaviour in a constant expression that every product evaluates. It has "
    "been correct since then only because the wraparound happens to land on the right value: the "
    "test reads the macro back as a uint32 and gets a number ABOVE INT_MAX, which is only possible "
    "because the int it was computed in went negative. arm-none-eabi-gcc 15.2.1 compiles it "
    "silently even with -Woverflow, which is why nobody has seen it. Two other tools do not: clang "
    "reports -Winteger-overflow, and under this suite's -Werror that is a hard build failure, "
    "which is why the three accessors that mention the macro live in a CLOSURE file of their own "
    "(Test/alxIdTestDateComp.c); and UBSan reports it at runtime, measured word for word as "
    "\"signed integer overflow: 2000000000 + 600000000 cannot be represented in type 'int'\" - so "
    "the accessor that evaluates it carries a no_sanitize attribute, because -fno-sanitize-recover "
    "makes that finding an abort that takes the whole runner with it and this seal says the same "
    "thing without killing the lane. The fix is one suffix - make the multipliers unsigned long, "
    "or build the value from uint32_t terms"))
def test_ALX1553_P481_the_compile_stamp_is_computed_in_a_type_that_can_hold_it(id_lib):
    """The macro's own int arithmetic should represent the date it computes.

    A YYMMDDHHmm value is ten decimal digits and passed 2147483647 in the year 2022, so the
    expression has been overflowing for four years while producing the correct answer. A value read
    back above INT_MAX is the proof that the int it was computed in did not hold it.
    """
    assert id_lib.build("DateComp") <= 0x7FFFFFFF, (
        "ALX_BUILD_DATE_COMP overflowed the int its own arithmetic is done in")


# =====================================================================
# The second identity block: the bootloader's, read out of flash
# =====================================================================


@pytest.mark.xfail(strict=True, reason=(
    "DEFECT: with fwIsBootUsed false, AlxId_Ctor writes me->fwIsBootUsed and me->fwBootIdAddr and "
    "then skips the whole block at alxId.c:132 - so me->fwBootId, 414 bytes of it, is never "
    "touched at all. There is no else branch and no memset outside the if. All ELEVEN boot getters "
    "still read it and still answer: AlxId_GetFwBootArtf, Name, VerStr and BinStr hand the caller "
    "a const char* into memory the constructor never wrote, with no terminating NUL guaranteed "
    "anywhere in the field. Measured on a 0xFF poisoned object: the 40-byte artf field contains no "
    "zero byte, so reading it as a string walks out of artf, through name, verStr and binStr, past "
    "the end of fwBootId entirely, and stops 424 bytes later at the first zero the constructor "
    "happened to write elsewhere in AlxId. In a product that is a several-hundred-character run of "
    "garbage printed as the bootloader's artifact name into the very trace the verification chain "
    "parses. It survives unnoticed because a product declares AlxId static or global, so the block "
    "lives in BSS and reads as clean zeros (P483). The fix is one memset in the else branch, or "
    "moving the existing memset at alxId.c:135 out of the if where it would do some good"))
def test_ALX1553_P482_an_identity_with_no_bootloader_does_not_hand_out_uninitialised_memory(id_lib):
    """With no bootloader declared, the boot artifact should at least be a terminated string.

    The object is poisoned with 0xFF first, which is what makes the question answerable: on a fresh
    allocation the same bytes read as an empty string and the getter looks perfectly well behaved.
    The field's own 40 bytes are read directly rather than through c_char_p, because c_char_p stops
    at the first zero and so cannot tell a field that ends inside itself from one that does not.
    """
    obj = _fw(id_lib, is_boot_used=False)

    field = _field_bytes(id_lib, obj, "FwBootArtf", NAME_LEN)

    assert b"\x00" in field, "the boot artifact field holds no terminator at all"


def test_ALX1553_P483_the_same_object_zero_filled_answers_cleanly_everywhere(id_lib):
    """CHARACTERIZATION: why P482 has never been seen - in a product the block is BSS, and BSS is zeros.

    A product declares its AlxId as a static or a global, so the 414 bytes the constructor never
    writes are zeros before main runs and every boot getter answers "" or 0. The identity trace looks
    right on every board that has ever been built, and the module's dependence on a storage duration
    it does not specify anywhere is invisible.

    Pinning the pair - poisoned garbage here, zeroed clean there - is what stops P482 being waved
    away as an artefact of the test harness. It is the harness that makes it VISIBLE; the missing
    else branch is what makes it real.
    """
    obj = _fw(id_lib, poison=0x00, is_boot_used=False)

    assert id_lib.text(obj, "FwBootArtf") == ""
    assert id_lib.text(obj, "FwBootName") == ""
    assert id_lib.text(obj, "FwBootVerStr") == ""
    assert id_lib.text(obj, "FwBootBinStr") == ""
    assert id_lib.num(obj, "FwBootVerMajor") == 0
    assert id_lib.num(obj, "FwBootVer") == 0
    assert id_lib.num(obj, "FwBootHashShort") == 0


def test_ALX1553_P484_a_valid_block_is_re_rendered_from_its_own_numbers_not_copied(id_lib):
    """The constructor recomputes verDate, the packed version and BOTH strings from the blob's fields.

    A bootloader lays down an AlxId_FwBootId in flash and the running firmware copies it in. What is
    not obvious from the header is that the two rendered strings in that record are DISCARDED: the
    constructor sprintf's its own from the numeric fields beside them. The blob written here leaves
    verStr and binStr as the zeros the writer put there, so a non-empty answer can only have been
    rendered by the constructor - and the shape it renders is the same wire format the running
    firmware's own block uses (P473), which is what lets one parser read both sections of a trace.

    A bootloader author reading the header would reasonably assume the strings they stored are what
    gets reported. They are not, and nothing says so.
    """
    addr = id_lib.flash()
    id_lib.boot_blob(addr, **BOOT)
    obj = _fw(id_lib, is_boot_used=True, boot_addr=addr)

    assert id_lib.text(obj, "FwBootVerStr") == f"4.5.6.2401021200.{BOOT_HASH}"
    assert id_lib.text(obj, "FwBootBinStr") == "2401021200_BOOT-ARTF_BOOT-NAME_V4-5-6_deadbee.bin"
    assert id_lib.num(obj, "FwBootVerDate") == 2401021200
    assert id_lib.num(obj, "FwBootVer") == _packed(4, 5, 6, 2401021200)
    assert id_lib.num(obj, "FwBootHashShort") == 0x0DEADBEE


def test_ALX1553_P485_the_blocks_own_build_job_flag_chooses_its_date(id_lib):
    """The blob's isBuildJobUsed picks between its build.date and its build.dateComp, exactly as P471.

    This is the one decision the second identity block makes on its own, and the two fields it picks
    between sit four bytes apart in the same struct. Whichever is chosen becomes verDate, which
    becomes the packed version and the leading field of the bootloader's .bin filename - so getting
    it backwards makes every derived field of the bootloader wrong at once while each one stays
    perfectly well formed, and there is no second source to check any of them against.
    """
    addr = id_lib.flash()
    id_lib.boot_blob(addr, is_build_job_used=False, **BOOT)
    off = _fw(id_lib, is_boot_used=True, boot_addr=addr)

    id_lib.boot_blob(addr, is_build_job_used=True, **BOOT)
    on = _fw(id_lib, is_boot_used=True, boot_addr=addr)

    assert id_lib.num(off, "FwBootVerDate") == 2401021200
    assert id_lib.num(on, "FwBootVerDate") == 2409091111
    assert id_lib.text(off, "FwBootBinStr").startswith("2401021200_")
    assert id_lib.text(on, "FwBootBinStr").startswith("2409091111_")


@pytest.mark.xfail(strict=True, reason=(
    "DEFECT: AlxId_Ctor memsets me->fwBootId to zero (alxId.c:135) and then, on the very next "
    "line, memcpys sizeof(me->fwBootId) bytes of flash over the top of it - so the memset is dead "
    "code, every byte it wrote is overwritten before anything reads it. Only THEN does it check "
    "magicNum against ALX_ID_BOOT_ID_MAGIC_NUM and break out at alxId.c:146. The break abandons "
    "the derived fields but leaves the raw ones exactly as flash had them, so every boot getter "
    "reports the REJECTED blob's contents as though it had been accepted: measured artf "
    "'BOOT-ARTF', name 'BOOT-NAME', version 4.5.6, short hash 0x0DEADBEE. The block even "
    "contradicts itself, since verStr and binStr were never rendered and come back empty beside a "
    "major version of 4. Nothing tells the caller: GetFwIsBootUsed still answers true, the header "
    "has no validity getter, and no assertion is recorded (P488). The fix is to move the memset "
    "after the two checks, or to clear the struct on the break"))
def test_ALX1553_P486_a_block_with_the_wrong_magic_number_is_not_reported_as_a_bootloader(id_lib):
    """A block the constructor rejected should leave nothing behind for the getters to report.

    Erased flash is the realistic case and it is worse (P489); a plausible-looking blob with the
    wrong magic is the clearest one, because every field is readable and every field is wrong.
    """
    addr = id_lib.flash()
    id_lib.boot_blob(addr, magic_num=0xDEADDEAD, **BOOT)
    obj = _fw(id_lib, is_boot_used=True, boot_addr=addr)

    assert id_lib.text(obj, "FwBootArtf") == "", "a rejected block was reported as the bootloader"


@pytest.mark.xfail(strict=True, reason=(
    "DEFECT: the same dead memset as P486, reached through the second gate. A block whose "
    "magicNum is right but whose ver is not ALX_ID_BOOT_ID_VER breaks at alxId.c:152 with its raw "
    "bytes already copied in, and every boot getter then reports them under the VERSION 1 field "
    "offsets. That is the forward-compatibility path and it is the one a real deployment meets "
    "first: the day a bootloader starts writing an AlxId_FwBootId version 2 with a different "
    "layout, every fielded product reinterprets those bytes under the old offsets and prints the "
    "result as the bootloader's identity. The version check exists precisely to protect against a "
    "layout change and then throws its own answer away. Measured: ver 2 gives artf 'BOOT-ARTF', "
    "version 4.5.6, short hash 0x0DEADBEE - indistinguishable from an accepted block"))
def test_ALX1553_P487_a_block_of_an_unknown_version_is_not_reinterpreted_under_the_old_layout(id_lib):
    """A record version the module does not understand should not be read as one it does."""
    addr = id_lib.flash()
    id_lib.boot_blob(addr, ver=id_lib.BOOT_ID_VER + 1, **BOOT)
    obj = _fw(id_lib, is_boot_used=True, boot_addr=addr)

    assert id_lib.text(obj, "FwBootArtf") == "", "a future block was reported under version 1 offsets"


def test_ALX1553_P488_nothing_in_the_public_header_can_tell_a_caller_the_block_was_rejected(id_lib):
    """CHARACTERIZATION: the module performs the check that would let it warn, and then says nothing.

    This is what turns P486 and P487 from ugly into dangerous. AlxId_GetFwIsBootUsed still returns
    true, because it is a straight copy of the constructor's argument and no other line in the
    module ever writes that field - it is a DECLARATION by the product that a bootloader exists,
    never a discovery. There is no validity getter, no status return and not even an ALX_ID_ASSERT,
    in a module that asserts freely on far smaller things. The assertion counter measures zero.

    The one observable trace of the rejection is a self-contradiction: verStr and binStr were never
    rendered, so they come back empty beside a major version of 4 and a nine-character artifact
    name. A caller would have to know to look for that, and know what it meant.

    Stated once here rather than three times, so the suite carries one statement of the missing API
    and the two xfails above carry the defect.
    """
    addr = id_lib.flash()
    id_lib.boot_blob(addr, magic_num=0xDEADDEAD, **BOOT)
    obj = _fw(id_lib, is_boot_used=True, boot_addr=addr)

    assert id_lib.num(obj, "FwIsBootUsed") is True, "the only flag a caller has stopped saying true"
    assert id_lib.c.AlxAssertPc_Count() == 0, "a rejected block now records something after all"
    assert id_lib.text(obj, "FwBootArtf") == "BOOT-ARTF"
    assert id_lib.num(obj, "FwBootVerMajor") == 4
    assert id_lib.text(obj, "FwBootVerStr") == "", "the derived fields were rendered after all"


@pytest.mark.xfail(strict=True, reason=(
    "DEFECT: the field case of P486. A product built with fwIsBootUsed true, running on a board "
    "whose boot sector was never programmed - a first bring-up, or any board after a full chip "
    "erase - reads 414 bytes of 0xFF. magicNum is 0xFFFFFFFF, the block is rejected at "
    "alxId.c:146, and the dead memset leaves all 414 bytes of 0xFF in me->fwBootId. Every "
    "character field is then an unterminated run: measured, the 40-byte artf field contains no "
    "zero byte and reading it as a string walks 424 bytes before finding one. AlxId_Trace prints "
    "it with %s straight into the boot trace, where alxTrace's 256-byte vsnprintf truncation is "
    "the only thing bounding it, and the identity that was supposed to say which image ran becomes "
    "several hundred bytes of 0xFF. Same fix as P486 - the memset belongs after the checks"))
def test_ALX1553_P489_an_erased_boot_sector_does_not_become_an_unterminated_string(id_lib):
    """Erased flash under a bootloader-configured image should not leave a string with no end.

    The assertion is on the field's own 40 bytes rather than on how far the read actually runs: the
    run length depends on which zero byte the constructor happened to write later in AlxId and is
    not a property of this module. That there is no terminator inside the field is.
    """
    addr = id_lib.flash()
    id_lib.flash_fill(addr, 0xFF)
    obj = _fw(id_lib, is_boot_used=True, boot_addr=addr)

    field = _field_bytes(id_lib, obj, "FwBootArtf", NAME_LEN)

    assert field == b"\xff" * NAME_LEN, "the erased bytes were not what reached the getter"
    assert b"\x00" in field, "the boot artifact field holds no terminator at all"


@pytest.mark.parametrize(("case", "unterminated"), [
    ("both name fields full", False),
    ("an artifact with no terminator", True),
])
@pytest.mark.xfail(strict=True, reason=(
    "DEFECT: the P475 overflow again, on the boot path (alxId.c:172), and worse here because the "
    "inputs are not the product's own constructor arguments - they are whatever is in flash. "
    "binStr is char[ALX_ID_FW_BIN_STR_LEN] = 96 and the format interpolates two char[40] fields "
    "from the blob; 39 characters plus a NUL in each is exactly what the struct is sized for, and "
    "with a 10-digit date and three 3-digit version fields that renders 115 characters - 19 past "
    "the end of binStr, which is the last member of the packed AlxId_Fw inside AlxId_FwBootId, so "
    "the surplus lands in fwBootId.crc (observed as the '22' tail of 'V111-222-233'). A field that "
    "flash filled to all 40 bytes with no terminator - which flash can legally contain and which "
    "the module never checks - renders 149. Nothing truncates and nothing asserts. It stays inside "
    "the AlxId object, so neither ASan nor UBSan will ever report it. The fix is snprintf, here and "
    "at alxId.c:124"))
def test_ALX1553_P490_the_bootloaders_bin_filename_fits_its_field(id_lib, case, unterminated):
    """The rendered filename should terminate inside its own 96 bytes for any blob the struct permits.

    Two shapes, because they fail for different reasons. Both name fields full is a legal record a
    careful bootloader could write; an artifact field with no terminator is a record the module
    never validates and flash can hold after a partial write or an erase.
    """
    addr = id_lib.flash()
    id_lib.boot_blob(addr, artf="A" * (NAME_LEN - 1), name="N" * (NAME_LEN - 1),
                     fw_ver=(111, 222, 233), build_date_comp=2412312359,
                     hash_=BOOT_HASH, hash_short="S" * 7)
    if unterminated:
        id_lib.boot_blob_fill(addr, "artf", ord("A"), NAME_LEN)
    obj = _fw(id_lib, is_boot_used=True, boot_addr=addr)

    assert b"\x00" in _field_bytes(id_lib, obj, "FwBootBinStr", BIN_STR_LEN), (
        f"{case}: the bootloader's .bin filename ran off the end of its field")


def test_ALX1553_P491_the_blocks_crc_is_never_computed_and_never_checked(id_lib):
    """CHARACTERIZATION: magicNum plus ver is the whole of the validation, and crc is decoration.

    AlxId_FwBootId declares a uint16 crc as its last member and no line of alxId.c reads it, writes
    it or computes one. A record that carries a checksum invites every reader to assume its contents
    were checked; combined with the retain-on-reject behaviour above, a blob with intact magic and
    version but a corrupted body is reported with full confidence and no way to doubt it.

    Two blobs identical but for the crc, reported identically, is the cheapest way to say so. The
    module never claimed to check it, which is why this is a characterization and not a seal - but
    the field's existence is a claim of its own.
    """
    addr = id_lib.flash()
    id_lib.boot_blob(addr, crc=0xBEEF, **BOOT)
    wrong = _fw(id_lib, is_boot_used=True, boot_addr=addr)
    reported = (id_lib.text(wrong, "FwBootVerStr"), id_lib.text(wrong, "FwBootBinStr"))

    id_lib.boot_blob(addr, crc=0x0000, **BOOT)
    zero = _fw(id_lib, is_boot_used=True, boot_addr=addr)

    assert reported == (id_lib.text(zero, "FwBootVerStr"), id_lib.text(zero, "FwBootBinStr"))
    assert id_lib.c.AlxAssertPc_Count() == 0, "a wrong crc was noticed after all"


def test_ALX1553_P492_both_constructors_read_the_boot_block_the_same_way(id_lib):
    """AlxId_Ctor_NoHwId forwards fwIsBootUsed and fwBootIdAddr unchanged and delegates the rest.

    Every other bootloader test here is written against the no-hardware-id constructor, because it
    is the one that needs no straps - which makes the delegation load-bearing for the whole area. If
    a later change gave AlxId_Ctor_NoHwId a body of its own, the boot block could quietly stop being
    read and every one of those tests would still pass. This is the test that stops that, and the
    only place in this file the full constructor and the delegating one are put side by side over
    the same 414 bytes of flash.
    """
    addr = id_lib.flash()
    id_lib.boot_blob(addr, **BOOT)

    full = _board(id_lib, [id_lib.LO, id_lib.HI_Z], [(2, "BoardTwo")], [2],
                  is_boot_used=True, boot_addr=addr)
    id_lib.init(full)
    delegated = _fw(id_lib, is_boot_used=True, boot_addr=addr)

    numbers = ("FwIsBootUsed", "FwBootVerMajor", "FwBootVerMinor", "FwBootVerPatch",
               "FwBootVerDate", "FwBootVer", "FwBootHashShort")
    strings = ("FwBootArtf", "FwBootName", "FwBootVerStr", "FwBootBinStr")

    assert [id_lib.num(full, n) for n in numbers] == [id_lib.num(delegated, n) for n in numbers]
    assert [id_lib.text(full, s) for s in strings] == [id_lib.text(delegated, s) for s in strings]


# =====================================================================
# The board: identification straps, and what Init does with them
# =====================================================================


@pytest.mark.parametrize(("straps", "hw_id"), [
    (("HI_Z",), 0),
    (("HI",), 1),
    (("LO",), 2),
    (("LO", "HI_Z"), 2),
    (("HI_Z", "LO"), 6),
    (("HI", "LO", "HI_Z"), 7),
    (("LO", "LO", "LO", "LO", "LO"), 242),
])
def test_ALX1553_P493_the_straps_spell_the_board_id_in_base_three_lowest_pin_first(id_lib, straps, hw_id):
    """Each strap is a base-3 digit, pin 0 worth 1, pin 1 worth 3, pin 2 worth 9, and so on.

    The identification straps are how one firmware image tells which of several board variants it is
    running on, and the id it computes selects a whole AlxId_HwInstance - the pcb name, the bom, the
    versions. A transposed pair of straps resolves a different board and traces a different identity,
    which is precisely the failure the straps exist to prevent, and both answers look like readings
    because they ARE readings, just of each other's pin.

    Driven through AlxId_CalcHwId directly, which is a free function taking the pin array rather than
    an AlxId, so the weights are checked without an Init around them. All-Lo on five straps is 242,
    the largest legitimate id there is.
    """
    obj = id_lib.alloc()
    id_lib.ctor_no_hw_id(obj)
    for i, state in enumerate(straps):
        id_lib.strap(obj, i, getattr(id_lib, state))

    assert id_lib.calc_hw_id(obj, len(straps)) == hw_id


@pytest.mark.parametrize(("straps", "clash"), [
    (("UNDEFINED",), ("HI_Z", "HI")),
    (("HI_Z", "HI", "HI", "HI_Z", "UNDEFINED"), None),
])
@pytest.mark.xfail(strict=True, reason=(
    "DEFECT: AlxId_CalcHwId folds the strap readings as base-3 digits - _hwId += state * pow3 with "
    "pow3 stepping 1, 3, 9, 27, 81 - but AlxIoPin_TriState has FOUR values, and "
    "AlxIoPin_TriState_Undefined is 3, which is exactly the weight of the next digit. An alphabet "
    "of four in a base of three: one Undefined strap and the two-strap pattern (HiZ, Hi) both "
    "compute id 3, so two different boards resolve to the same AlxId_HwInstance and each traces "
    "the other's pcb identity. Undefined is not a fault value - it is what AlxIoPin_Read_TriState "
    "returns when the pull-up read and the pull-down read disagree in the inverted direction, "
    "which a floating or slowly settling strap does. The uint8_t accumulator then wraps: five "
    "Undefined straps give 107, inside the legitimate range, and (HiZ, Hi, Hi, HiZ, Undefined) "
    "gives exactly 255 - the value AlxId_Ctor writes into instance.id as its own 'Error' sentinel, "
    "so a real strap pattern is indistinguishable from a board that was never identified. The fix "
    "is base 4, or an assertion that no strap read Undefined"))
def test_ALX1553_P494_a_strap_that_reads_undefined_does_not_alias_another_pattern(id_lib, straps, clash):
    """Distinct strap patterns should compute distinct ids, and none should reach the error sentinel.

    Two cases, one per consequence. The first is the aliasing: a single Undefined strap answering
    the same id as a two-strap pattern of ordinary readings. The second is the wraparound landing on
    255, which the constructor has already reserved to mean "not identified".
    """
    obj = id_lib.alloc()
    id_lib.ctor_no_hw_id(obj)
    for i, state in enumerate(straps):
        id_lib.strap(obj, i, getattr(id_lib, state))
    hw_id = id_lib.calc_hw_id(obj, len(straps))

    if clash is None:
        assert hw_id != 255, "a legal strap pattern computed the constructor's own error sentinel"
        return

    other = id_lib.alloc()
    id_lib.ctor_no_hw_id(other)
    for i, state in enumerate(clash):
        id_lib.strap(other, i, getattr(id_lib, state))

    assert hw_id != id_lib.calc_hw_id(other, len(clash)), f"{straps} and {clash} computed one id"


def test_ALX1553_P495_init_configures_reads_and_releases_every_strap_exactly_once(id_lib):
    """Each strap is initialised, read as a tri-state and de-initialised, once, and left released.

    The straps are shared board resources - a product reuses those pins for something else once it
    has identified itself - so a missing DeInit strands a pull configured on a pin somebody else now
    owns, and a missing Init reads a pin that was never configured as an input. On the target each
    read also costs about 2 ms inside AlxIoPin_Read_TriState, which pulls the pin up, waits, reads,
    pulls it down, waits and reads again; alxId.c itself contains no delay at all.

    The state array Init filled is checked as well, because it is an [in,out] parameter the module
    keeps afterwards - and its untouched entries still hold the Undefined the constructor wrote,
    which is what makes a strap that was never read distinguishable from one that read HiZ.
    """
    obj = _board(id_lib, [id_lib.LO, id_lib.HI_Z], [(2, "BoardTwo")], [2])
    id_lib.init(obj)

    assert [id_lib.pin_counts(obj, i) for i in range(2)] == [(1, 1), (1, 1)]
    assert [id_lib.pin_state(obj, i) for i in range(3)] == [id_lib.LO, id_lib.HI_Z, id_lib.UNDEFINED]
    assert id_lib.num(obj, "HwId") == 2


@pytest.mark.xfail(strict=True, reason=(
    "DEFECT: both of AlxId_Init's lookups put their failure handling INSIDE the loop body, guarded "
    "by `if (i == len - 1)` - the not-found assertion at alxId.c:443 and the not-supported one at "
    "alxId.c:460. With a zero-length array the loop body never executes, so there is no iteration "
    "in which either can fire: Init reads the straps, matches nothing against nothing, checks "
    "support against nothing, and runs all the way through to isInit = true. Measured on a 0xFF "
    "poisoned object with both lengths zero: no assertion recorded, isInit true, id 2 from the "
    "straps, and AlxId_GetHwPcbVerStr answering '255.255.255.4294967295' - a fully initialised "
    "identity composed out of uninitialised memory. The supported array is the plausible one to "
    "leave empty (a product meaning 'no restriction'), and it silently means 'no check'. The fix "
    "is to test the lengths before the loops rather than inside them"))
def test_ALX1553_P496_a_board_that_matched_nothing_does_not_come_up_initialised(id_lib):
    """An identity that reads its straps and finds no board should not report itself initialised.

    With both tables empty nothing CAN match, which is the clearest possible case of the lookup
    failing - and it is the one case the lookup cannot notice.
    """
    obj = _board(id_lib, [id_lib.LO, id_lib.HI_Z], [], [])
    id_lib.init(obj)

    assert not id_lib.is_init(obj), "Init finished over a board it never resolved"


@pytest.mark.expect_assert(
    "an id that is in no known instance is the failure P497 characterises, and the getters called "
    "afterwards each add their own isInit assertion on the object Init abandoned"
)
def test_ALX1553_P497_an_unknown_board_leaves_the_object_abandoned_but_answering(id_lib):
    """Init asserts, returns, and leaves an object nothing but isInit can tell apart from a good one.

    The assertion and the return are separate statements, so the return fires whether or not
    assertions were compiled in - and the library's default compiles them to do{}while(false), which
    means a shipping product takes this path in silence. What is left behind is not neutral: the id
    the straps computed is already written into the instance, so AlxId_GetHwId answers 2 as
    confidently as ever, while the version strings Init returned before rendering are still whatever
    was in memory. Every getter afterwards asserts isInit and every one of them answers anyway.

    Recorded text: "RST alxId.h:443 in AlxId_Init" - ALX_ID_FILE is the header's name, the line is
    alxId.c's.
    """
    obj = _board(id_lib, [id_lib.LO, id_lib.HI_Z], [(7, "BoardSeven"), (4, "BoardFour")], [7])
    id_lib.init(obj)

    assert id_lib.c.AlxAssertPc_Count() == 1
    assert (id_lib.c.AlxAssertPc_First() or b"").decode("ascii") == "RST alxId.h:443 in AlxId_Init"
    assert not id_lib.is_init(obj)
    assert id_lib.num(obj, "HwId") == 2, "the id that matched nothing is still reported"
    assert _field_bytes(id_lib, obj, "HwPcbVerStr", 16) == b"\xff" * 16, "Init rendered a version"


@pytest.mark.expect_assert(
    "a board that is known but not supported is the failure P498 characterises; the getters after "
    "it each add their own isInit assertion"
)
def test_ALX1553_P498_an_unsupported_board_is_left_half_resolved(id_lib):
    """The instance copy has already happened when the support check fails, so the object is a mixture.

    The two lookups are separate loops: the first copies the WHOLE matching AlxId_HwInstance over
    me->hw.instance, the second then checks the id against the supported list and returns if it is
    not there. A support failure cannot undo the copy, so the object reports a correct board name and
    a correct id next to version strings that were never rendered. That reads to a caller as a data
    problem rather than as what it is - this firmware refusing to run on this board - and on a
    product with assertions compiled out there is no other signal at all.

    Recorded text: "RST alxId.h:460 in AlxId_Init".
    """
    obj = _board(id_lib, [id_lib.LO, id_lib.HI_Z], [(7, "BoardSeven"), (2, "BoardTwo")], [7])
    id_lib.init(obj)

    assert id_lib.c.AlxAssertPc_Count() == 1
    assert (id_lib.c.AlxAssertPc_First() or b"").decode("ascii") == "RST alxId.h:460 in AlxId_Init"
    assert not id_lib.is_init(obj)
    assert id_lib.text(obj, "HwPcbName") == "BoardTwo", "the instance copy did not happen"
    assert id_lib.num(obj, "HwId") == 2
    assert _field_bytes(id_lib, obj, "HwPcbVerStr", 16) == b"\xff" * 16, "Init rendered a version"


def test_ALX1553_P499_the_board_versions_are_composed_at_init_from_the_instance_that_was_resolved(id_lib):
    """pcbVer and bomVer are built by Init, with the same layout and the same 8-bit hole as the firmware's.

    Two claims in one, because they are the same statement. The layout is the third verbatim copy of
    the composition at alxId.c:116 - major<<56, minor<<48, patch<<32, date in the low word, bits
    40..47 unreachable - so a decoder written for one works on all three and a decoder written for
    the field order works on none. And the values come from the instance the lookup RESOLVED, not
    from anything the constructor was given: a product that pre-filled pcbVer in its static table
    would have it silently discarded, which is what the poison standing in for a pre-filled value
    shows here.
    """
    obj = _board(id_lib, [id_lib.LO, id_lib.HI_Z], [(2, "BoardTwo")], [2])
    id_lib.set_pcb(id_lib.known(obj, 0), hw_id=2, name="BoardTwo", ver=(1, 0, 0), date=2601020304)
    id_lib.set_bom(id_lib.known(obj, 0), name="BomTwo", ver=(2, 3, 4), date=2512250000)
    id_lib.init(obj)

    pcb, bom = id_lib.num(obj, "HwPcbVer"), id_lib.num(obj, "HwBomVer")

    assert pcb == _packed(1, 0, 0, 2601020304), "the pre-filled value survived, or the layout moved"
    assert bom == _packed(2, 3, 4, 2512250000)
    assert (pcb >> 40) & 0xFF == 0
    assert (bom >> 40) & 0xFF == 0
    assert id_lib.text(obj, "HwPcbVerStr") == "1.0.0.2601020304"
    assert id_lib.text(obj, "HwBomVerStr") == "2.3.4.2512250000"


def test_ALX1553_P500_every_field_of_the_resolved_board_is_reported_by_its_own_getter(id_lib):
    """Sixteen getters over two near-identical halves, each driven to a value only it can be holding.

    AlxId_HwInstance carries a pcb half and a bom half with the same seven field names, and the
    sixteen getters that read them are the same four lines of code sixteen times over. That is
    exactly the shape a copy-paste error survives in - AlxId_GetHwBomVerMinor returning
    bomVerMajor, or the pcb getter reading the bom field - and nothing about such a getter looks
    wrong: it compiles, it returns a plausible number, and every board reports it consistently. The
    module already carries one visible scar of the copying at alxId.c:1164, a doubled semicolon
    after the bomVerDate return.

    Every field is driven to a value that appears nowhere else in the object, so a getter reading
    its neighbour's member reports a number that cannot be confused with its own. Run through the
    strap path so the fields also have to survive the whole-instance copy the lookup performs.
    """
    obj = _board(id_lib, [id_lib.LO, id_lib.HI_Z], [(2, "BoardTwo")], [2])
    id_lib.set_pcb(id_lib.known(obj, 0), hw_id=2, artf="PcbArtfTwo", name="PcbNameTwo",
                   ver=(11, 22, 33), date=2601020304)
    id_lib.set_bom(id_lib.known(obj, 0), artf="BomArtfTwo", name="BomNameTwo",
                   ver=(44, 55, 66), date=2512250000)
    id_lib.init(obj)

    pcb = [id_lib.text(obj, "HwPcbArtf"), id_lib.text(obj, "HwPcbName"),
           id_lib.num(obj, "HwPcbVerMajor"), id_lib.num(obj, "HwPcbVerMinor"),
           id_lib.num(obj, "HwPcbVerPatch"), id_lib.num(obj, "HwPcbVerDate")]
    bom = [id_lib.text(obj, "HwBomArtf"), id_lib.text(obj, "HwBomName"),
           id_lib.num(obj, "HwBomVerMajor"), id_lib.num(obj, "HwBomVerMinor"),
           id_lib.num(obj, "HwBomVerPatch"), id_lib.num(obj, "HwBomVerDate")]

    assert pcb == ["PcbArtfTwo", "PcbNameTwo", 11, 22, 33, 2601020304]
    assert bom == ["BomArtfTwo", "BomNameTwo", 44, 55, 66, 2512250000]
    assert id_lib.num(obj, "HwId") == 2
    assert id_lib.text(obj, "HwPcbVerStr") == "11.22.33.2601020304"
    assert id_lib.text(obj, "HwBomVerStr") == "44.55.66.2512250000"
    assert id_lib.num(obj, "HwPcbVer") == _packed(11, 22, 33, 2601020304)
    assert id_lib.num(obj, "HwBomVer") == _packed(44, 55, 66, 2512250000)


def test_ALX1553_P501_the_no_strap_constructor_takes_its_board_verbatim_and_reads_no_pin(id_lib):
    """isHwIdUsed gates the whole resolution block, and AlxId_Ctor_NoHwId is the only thing that clears it.

    This is the path most products take - one board variant, no identification straps - and the only
    one where Init does no resolution and can therefore never assert. The ordering inside
    AlxId_Ctor_NoHwId is what makes it work: it delegates to AlxId_Ctor, which sets isHwIdUsed TRUE,
    and only then sets it false. Reorder those two and a product with no straps at all walks into
    AlxId_CalcHwId with a NULL pin array.

    The companion case is the trap beside it: the full constructor with a pin count of zero does NOT
    behave the same. It runs the resolution, reads no pins, and computes id 0 - a different board,
    from what reads in a header like the same configuration.
    """
    obj = id_lib.alloc()
    id_lib.set_pcb(id_lib.instance(obj), hw_id=42, name="TheOnlyBoard", ver=(1, 0, 0), date=2601020304)
    id_lib.set_bom(id_lib.instance(obj))
    id_lib.ctor_no_hw_id(obj)
    id_lib.init(obj)

    assert not id_lib.is_hw_id_used(obj)
    assert id_lib.num(obj, "HwId") == 42, "the instance's own id was overwritten"
    assert id_lib.text(obj, "HwPcbVerStr") == "1.0.0.2601020304"
    assert [id_lib.pin_counts(obj, i) for i in range(2)] == [(0, 0), (0, 0)], "a strap was touched"

    no_pins = _board(id_lib, [], [(0, "BoardZero")], [0])
    id_lib.init(no_pins)

    assert id_lib.is_hw_id_used(no_pins)
    assert id_lib.num(no_pins, "HwId") == 0, "a resolution over no straps did not compute zero"


@pytest.mark.expect_assert(
    "a second Init on an initialised object is the isInit == false assertion, which is what P502 "
    "characterises - the module flags re-entry and then performs it"
)
def test_ALX1553_P502_a_second_init_re_reads_the_straps_and_takes_the_new_answer(id_lib):
    """CHARACTERIZATION: re-entry is flagged, not blocked, and the flag is compiled out by default.

    ALX_ID_ASSERT(me->isInit == false) is the entire guard, and it is a do{}while(false) in the
    library's default configuration - so a product that calls Init twice, after a bootloader hands
    over or after a brown-out, simply runs the whole thing again. Every strap is reconfigured, read
    and released a second time, which on the target is another 2 ms per strap spent driving pins the
    product may since have repurposed, and the object takes whatever the pins say the second time.

    Pinned rather than sealed: re-initialising is a defensible thing for an object to allow. What is
    worth having on the record is that it really does re-read the hardware rather than short-circuit,
    so the second answer is the current one and not a cached one.
    """
    obj = _board(id_lib, [id_lib.LO, id_lib.HI_Z], [(2, "BoardTwo"), (1, "BoardOne")], [2, 1])
    id_lib.init(obj)
    assert id_lib.num(obj, "HwId") == 2

    id_lib.strap(obj, 0, id_lib.HI)
    id_lib.init(obj)

    assert id_lib.c.AlxAssertPc_Count() == 1
    assert (id_lib.c.AlxAssertPc_First() or b"").decode("ascii") == "RST alxId.h:418 in AlxId_Init"
    assert id_lib.num(obj, "HwId") == 1, "the second Init did not re-read the straps"
    assert [id_lib.pin_counts(obj, i) for i in range(2)] == [(2, 2), (2, 2)]


@pytest.mark.expect_assert(
    "more straps than ALX_ID_HW_ID_IO_PIN_ARR_LEN is exactly the precondition AlxId_CalcHwId "
    "asserts against - P503 characterises what it does afterwards"
)
def test_ALX1553_P503_the_strap_count_is_asserted_and_then_not_enforced(id_lib):
    """CHARACTERIZATION: the length check records the violation and the loop runs to completion anyway.

    ALX_ID_ASSERT(hwIdIoPinArrLen <= ALX_ID_HW_ID_IO_PIN_ARR_LEN) is the only guard in the function
    and it does not clamp, does not return and does not shorten the loop. The state array it is
    handed by AlxId_Init is me->hw.idIoPinState, exactly five entries and the last member of
    AlxId_Hw, so a sixth strap writes an int past it - on this host over the three lifecycle flags
    that follow, on the target into hwStm32. And in the library's default configuration the
    assertion is compiled away entirely, so a product that grows a sixth strap has no guard at all.

    Six straps are driven here rather than reached through AlxId_Init on purpose: the sixth is left
    floating so the value that lands past the array is zero, and the object is not read again.

    Recorded text: "RST alxId.h:1283 in AlxId_CalcHwId".
    """
    obj = id_lib.alloc()
    id_lib.ctor_no_hw_id(obj)
    for i in range(5):
        id_lib.strap(obj, i, id_lib.LO)
    id_lib.strap(obj, 5, id_lib.HI_Z)

    hw_id = id_lib.calc_hw_id(obj, 6)

    assert id_lib.c.AlxAssertPc_Count() == 1
    assert (id_lib.c.AlxAssertPc_First() or b"").decode("ascii") == "RST alxId.h:1283 in AlxId_CalcHwId"
    assert hw_id == 242, "the fold stopped at the fifth strap after all"
    assert [id_lib.pin_counts(obj, i) for i in range(6)] == [(1, 1)] * 6, "a strap past the limit was skipped"


# =====================================================================
# The lifecycle rule, stated where it is least expected
# =====================================================================


@pytest.mark.expect_assert(
    "tracing before Init is the assertion P504 characterises - the one observable thing AlxId_Trace "
    "does in a build with ALX_TRACE_LEVEL_OFF"
)
def test_ALX1553_P504_trace_is_gated_on_init_and_renders_nothing_in_this_configuration(id_lib):
    """CHARACTERIZATION: in this build AlxId_Trace is its two assertions and nothing else.

    Test/alxConfig.h sets ALX_TRACE_LEVEL to ALX_TRACE_LEVEL_OFF, which compiles ALX_TRACE_INF
    itself away, so every one of the thirty-odd lines the function would write is gone before
    ALX_ID_TRACE_ENABLE is even consulted. What is left is the lifecycle rule, and it is worth
    pinning because it is the same rule the getters follow for data the constructor already
    finished: Trace reads nothing Init produced except pcbVerStr and bomVerStr, and still refuses
    to run before Init.

    The wire shape the Python identity parser depends on - the "FW:" and "FW - Bootloader:" section
    texts, the "- key: value" separator, the artf/name/ver/bin keys and the INF level that keeps
    alxTrace from prefixing each line with its file and function - is NOT tested here and cannot be
    from this group. Proving it needs the real Mcu/alxTrace.c linked in and alxConfig.h's trace
    level made overridable. The four values those lines are formatted from are pinned through their
    getters instead (P473, P484).

    Recorded text: "RST alxId.h:510 in AlxId_Trace".
    """
    before_init = _fw_ctor(id_lib)
    id_lib.trace(before_init)

    assert id_lib.c.AlxAssertPc_Count() == 1
    assert (id_lib.c.AlxAssertPc_First() or b"").decode("ascii") == "RST alxId.h:510 in AlxId_Trace"

    after_init = _fw(id_lib)
    id_lib.trace(after_init)

    assert id_lib.c.AlxAssertPc_Count() == 1, "tracing an initialised identity asserted or faulted"


@pytest.mark.expect_assert(
    "the two array variants of the MCU unique id assert(false) on a non-STM32 build, and reading "
    "the firmware artifact before Init trips the isInit assertion this getter does not make - both "
    "are the contrasts P505 is built on"
)
def test_ALX1553_P505_one_fact_reached_through_three_getters_with_three_different_contracts(id_lib):
    """CHARACTERIZATION: the MCU's unique id has a getter that answers, one that asserts, and one lifecycle all its own.

    AlxId_GetHwMcuUniqueIdStr asserts wasCtorCalled and stops there, unlike all forty getters
    around it, which assert isInit as well - so it is the one getter in the module that may be
    called before Init, and on a build without ALX_STM32 it returns a literal empty string rather
    than reading anything. Its two siblings behave differently again: _Uint32 and _Uint8 assert
    false on the same build and leave the caller's buffer exactly as they found it, so a caller
    that did not check gets whatever it had allocated.

    The third difference is in their signatures and outlives the non-STM32 case: both pass `len`
    straight to memcpy as a BYTE count, whatever unit their own name promises, so
    GetHwMcuUniqueIdUint32(me, buf, 3) copies three BYTES and not three words. Nothing in the
    header says so, and on an STM32 build it would quietly return a third of the id.

    Three getters for one fact, three contracts, none of it written down. Pinned as the asymmetry
    it is: the string arrives with no assertion, the arrays each record one, and the very next
    ordinary getter on the same object records another.
    """
    obj = _fw_ctor(id_lib)

    assert id_lib.text(obj, "HwMcuUniqueIdStr") == ""
    assert id_lib.c.AlxAssertPc_Count() == 0, "the unique id string asserted on isInit after all"

    assert id_lib.mcu_unique_id(obj, 32, 3) == [0xA5A5A5A5] * 3, "the buffer was written after all"
    assert id_lib.mcu_unique_id(obj, 8, 12) == [0xA5] * 12, "the buffer was written after all"

    assert id_lib.c.AlxAssertPc_Count() == 2, "the two array getters did not each assert"
    assert (id_lib.c.AlxAssertPc_First() or b"").decode("ascii").endswith("AlxId_GetHwMcuUniqueIdUint32")

    id_lib.text(obj, "FwArtf")

    assert id_lib.c.AlxAssertPc_Count() == 3, "every other getter should assert isInit"
