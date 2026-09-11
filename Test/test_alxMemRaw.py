"""ALX-1553 - alxMemRaw PC unit tests (Tier 1: the raw-memory contract, five weak symbols).

alxMemRaw is the only module in the library whose entire public API is ALX_WEAK - constructor,
Init, DeInit, Read and Write - and no file in the repository implements any of it. A product
supplies the implementation; on a board that keeps parameters outside flash it is the driver for
whatever device holds them. Every other ALX_WEAK in the library (alxAssert.c, alxBoot.c, alxCli.c,
alxLin.c) is a callback hook bolted onto a module that otherwise works. This one is the module.

Nothing had ever compiled it. The only Test/ file bearing the name is alxMemRawFake.c, a complete
strong replacement the MemSafe group links INSTEAD of the module, so every lane has been green over
a file none of them had seen.

The group is two DLLs because the symbols are weak and a strong definition displaces a weak one for
the whole image: ``mem_raw_lib`` keeps the library's five defaults and ``mem_raw_ovr_lib`` links a
product-shaped override of four of them, AlxMemRaw_DeInit deliberately left weak so one image shows
that displacement is per symbol.

Both DLLs are built with the module's assertions ON, which conftest derives from alxMemRaw.h, and
that is the thing to hold on to while reading anything here. It is NOT how the module ships. Without
that define the module's own macro is `do{} while (false)` and DISCARDS the expression, so a product
that forgets its override gets Alx_Ok, an untouched buffer and no diagnostic at all - and even with
the define the net needs a second, independent opt-in, because the library's own AlxAssert_Rst is an
empty body that returns (test_alxAssert.py P507). Two product-side decisions have to both go right
before a forgotten override produces any symptom at all.

Sealed defects: P516, P517, P518.

Proofs (ALX-1553): P516-P524.
"""

import pytest

pytestmark = pytest.mark.unit

# What the module's four asserting stubs record, verbatim. The file is ALX_MEM_RAW_FILE from
# alxMemRaw.h:56 and the line is __LINE__ from alxMemRaw.c - see P519 for what that pair means.
ASSERT_TEXT = {
    "Init": "RST alxMemRaw.h:70 in AlxMemRaw_Init",
    "DeInit": "RST alxMemRaw.h:83 in AlxMemRaw_DeInit",
    "Read": "RST alxMemRaw.h:101 in AlxMemRaw_Read",
    "Write": "RST alxMemRaw.h:120 in AlxMemRaw_Write",
}

# A payload a product would plausibly keep outside flash: a few parameters, nothing exotic.
PARAMS = b"PARAMS!!"


def _call(lib, me, which: str) -> int:
    """One of the four asserting stubs, by name, with arguments a caller would plausibly pass."""
    if which in ("Init", "DeInit"):
        return lib.lifecycle(me, which)
    if which == "Read":
        return lib.read(me, addr=0, length=len(PARAMS))
    return lib.write(me, addr=0, length=len(PARAMS))


# =====================================================================
# P516 - what the constructor leaves behind
# =====================================================================


@pytest.mark.xfail(strict=True, reason=(
    "DEFECT: the weak AlxMemRaw_Ctor (alxMemRaw.c:48-54) sets NEITHER of the two fields its own "
    "header declares. alxMemRaw.h:80-85 declares `bool wasCtorCalled` and `bool isInit` under an "
    "// Info comment, and the constructor body is `(void)me;` - it touches neither, and no function "
    "in the library ever reads either one for this module. That breaks the library's own convention "
    "in the module that defines the fields for it: alxMemSafe.c sets wasCtorCalled = true at line "
    "108 and then opens every public function with ALX_MEM_SAFE_ASSERT(me->wasCtorCalled == true) "
    "at lines 127, 366, 469, 484, 499 and 514, and alxCrc.c does the same at line 65. Measured on a "
    "0xFF poisoned object: after the constructor both fields still read back as the poison, and a "
    "constructed object is byte for byte an unconstructed one. The consequence lands on the product "
    "rather than the library, which is what makes it worth sealing: a product's override inherits a "
    "field contract nothing states and nothing checks, so an override that forgets wasCtorCalled - "
    "or that sets isInit true in the constructor - is never caught by anything. Test/alxMemRawFake.c "
    "guessed it right (wasCtorCalled = true, isInit = false) and nothing told it to. The fix is the "
    "two lines alxMemSafe_Ctor writes, or dropping the two fields from the header if the module "
    "genuinely does not want them"))
def test_ALX1553_P516_the_constructor_sets_the_two_fields_its_header_declares(mem_raw_lib):
    """The house 0xFF poison, and here it carries the whole test.

    The fields are bools, so on a zero-filled object - which is what a product gets, since AlxMemRaw
    is declared static or global and lives in BSS - a constructor that writes nothing is
    indistinguishable from one that writes `wasCtorCalled = false; isInit = false;`. The poison is
    what separates the two, and it says the constructor wrote nothing: the object it returns is the
    same object that went in.
    """
    untouched = mem_raw_lib.new_no_ctor()
    constructed = mem_raw_lib.new()
    both = ("WasCtorCalled", "IsInit")

    assert mem_raw_lib.SIZEOF == 2, "AlxMemRaw is meant to be the two bools and nothing else"
    assert [mem_raw_lib.field(constructed, f) for f in both] == [mem_raw_lib.POISON] * 2, (
        "the poison survived the constructor - it wrote neither field")
    assert [mem_raw_lib.field(untouched, f) for f in both] == [mem_raw_lib.field(constructed, f) for f in both], (
        "a constructed object differs from one whose constructor never ran")

    fields = tuple(mem_raw_lib.field(constructed, f) for f in both)
    assert fields == (1, 0), (
        "after a constructor wasCtorCalled is true and isInit is false - the convention alxMemSafe.c "
        f"and alxCrc.c set and check; these two bytes read {fields}, straight out of the 0xFF poison")


# =====================================================================
# P517 - a call that cannot work, reporting that it worked
# =====================================================================


@pytest.mark.xfail(strict=True, reason=(
    "DEFECT: all four of AlxMemRaw's public functions are `(void)me; ALX_MEM_RAW_ASSERT(false); "
    "return 0;` and 0 is Alx_Ok (alxGlobal.h:344). So every one of them asserts that it must not be "
    "called and then tells the caller it succeeded. This is the P466 shape (a lifecycle pair in "
    "alxTempSensRtdVdiv) with two differences that make it worse. It is the WHOLE module rather "
    "than a pair, and the caller acts on the value: alxMemSafe.c branches on `if (statusMemRaw != "
    "Alx_Ok)` at lines 156, 164, 172, 185, 193 and 201, so with the weak defaults in place AlxMemSafe "
    "takes the success branch on every one of them, runs AlxCrc_IsOk over a buffer nobody filled, "
    "and decides from the result which of two stored copies of the product's parameters is valid. "
    "The assertion is not the safety net it looks like either: the module ships with no "
    "ALX_MEM_RAW_ASSERT_*_ENABLE defined, which makes the macro do{} while (false), and even with it "
    "defined the library's own AlxAssert_Rst returns (test_alxAssert.py P507). A stub that cannot "
    "work should say so in its status - Alx_Err costs nothing and every caller already checks for "
    "it. The fix is one character per function"))
@pytest.mark.expect_assert(
    "P517: each stub is ALX_MEM_RAW_ASSERT(false) - tripping it is what identifies the library's "
    "own weak default as the definition that ran"
)
@pytest.mark.parametrize("call", ["Init", "DeInit", "Read", "Write"])
def test_ALX1553_P517_a_call_the_module_cannot_perform_does_not_report_success(mem_raw_lib, call):
    """Four stubs, one claim: the status a caller reads should not say Alx_Ok.

    The assertion count is checked first and passes, which is what pins WHICH definition ran - in
    the override image the same call reaches a product's code and records nothing (P520).
    """
    me = mem_raw_lib.new()
    mem_raw_lib.assert_reset()

    status = _call(mem_raw_lib, me, call)

    assert mem_raw_lib.asserts() == 1, f"AlxMemRaw_{call} did not reach the library's weak default"
    assert mem_raw_lib.first() == ASSERT_TEXT[call]
    assert status != mem_raw_lib.OK, (
        f"AlxMemRaw_{call} returned {status}, which is Alx_Ok - a call that asserted it must not "
        f"happen reported that it succeeded")


# =====================================================================
# P518 - the output parameter a read never writes
# =====================================================================


@pytest.mark.xfail(strict=True, reason=(
    "DEFECT: the weak AlxMemRaw_Read (alxMemRaw.c:98-103) never touches `data`, the only output "
    "parameter it has, and returns Alx_Ok. Measured: an 8 byte buffer filled with a recognisable "
    "pattern comes back byte for byte unchanged with the status reporting success. This is the "
    "serious half of P517 - a wrong status is at least a value a caller can test, and an untouched "
    "buffer is not distinguishable from a device that genuinely read those bytes back. alxMemSafe.c "
    "line 163 reads into me->buff1 and line 168 immediately runs AlxCrc_IsOk over it, so the module "
    "downstream computes a CRC over whatever was in the buffer already and decides from it whether "
    "the product's stored parameters are valid. The body does not even acknowledge the parameter: "
    "`me` is the only argument cast to void, and addr, data, len, numOfTries and timeout_ms are not "
    "mentioned at all, so nothing in the function says the output was considered. Same fix as P517 - "
    "return Alx_Err, and the question of what is in the buffer stops mattering"))
@pytest.mark.expect_assert(
    "P518: the weak AlxMemRaw_Read asserts on the way to returning Alx_Ok; the buffer is what is "
    "being measured"
)
def test_ALX1553_P518_a_read_that_reports_success_wrote_something_to_the_buffer(mem_raw_lib):
    """Fill the caller's buffer with a pattern, read over it, and see whether the pattern survived.

    Poison rather than zeros for the same reason as P516: a read that wrote nothing and a read that
    wrote eight zeros are the same buffer on a fresh allocation, and only one of them is a read.
    """
    me = mem_raw_lib.new()
    mem_raw_lib.fill(0xA5)
    mem_raw_lib.assert_reset()

    status = mem_raw_lib.read(me, addr=0, length=8)

    assert status == mem_raw_lib.OK, "the premise of this seal is that the caller was told it worked"
    assert mem_raw_lib.buff(8) != bytes([0xA5] * 8), (
        "AlxMemRaw_Read returned Alx_Ok and left all 8 bytes of the caller's buffer exactly as they "
        "were - the caller cannot tell that from a device that read those bytes back")


# =====================================================================
# P519 - what the assertion record actually says
# =====================================================================


@pytest.mark.expect_assert(
    "P519: all four stubs are driven so that all four records can be read back - the text is the "
    "measurement"
)
def test_ALX1553_P519_the_recorded_file_and_the_recorded_line_come_from_different_files(mem_raw_lib):
    """"RST alxMemRaw.h:101 in AlxMemRaw_Read" - and alxMemRaw.h:101 declares AlxMemRaw_DeInit.

    ALX_MEM_RAW_FILE is "alxMemRaw.h" (alxMemRaw.h:56) and __LINE__ expands in alxMemRaw.c, so the
    two halves of the location a field report carries come from two different translation-unit
    positions. Both are individually correct and the pair never names the assertion. Read's record
    lands on the DeInit declaration, which is the bad case: a reader who opens the file finds a
    plausible line for the wrong function. Write's lands on line 120 of a header that is 113 lines
    long, which is the honest case, because nothing is there to mislead anyone.

    Recorded here, sealed in test_alxAssert.py P513. It is a library-wide convention rather than
    this module's defect - all 91 ALX_*_FILE constants name a .h and none names a .c - so the group
    that owns the mechanism owns the seal, and this test's job is to show what it costs one module.
    The four strings are written out rather than derived, so moving a line in alxMemRaw.c makes this
    test say so.
    """
    me = mem_raw_lib.new()

    for call, expected in ASSERT_TEXT.items():
        mem_raw_lib.assert_reset()
        _call(mem_raw_lib, me, call)
        assert mem_raw_lib.first() == expected


# =====================================================================
# P520 - a product's override, and how far it reaches
# =====================================================================


@pytest.mark.expect_assert(
    "P520: AlxMemRaw_DeInit is left weak on purpose, so the one call in this image that still "
    "reaches the library trips its assertion - which is the measurement"
)
def test_ALX1553_P520_a_strong_definition_displaces_the_weak_one_per_symbol(mem_raw_ovr_lib):
    """Four calls reach the product's code and the fifth still reaches the library's stub, in one image.

    alxMemRawTestOverride.c is what a product writes. It defines Ctor, Init, Read and Write strongly
    and deliberately leaves DeInit alone, so this DLL holds both a displaced symbol and an
    undisplaced one and the difference between them is visible in the same run: the four record
    nothing and do the work, DeInit records its assertion and returns Alx_Ok.

    That is the ordinary case rather than a contrived one. A product implements the calls it needs
    and leaves the rest, and leaving one is exactly what puts a library stub on a shipped board's
    call path - with the return value saying Alx_Ok and, on the library's default configuration, no
    assertion at all to notice it by.

    Weak-symbol override on this host is newer than the module: alxGlobal.h:313-317 emits
    __attribute__((weak)) for clang on Windows only since this branch, and before that lld-link
    rejected an image holding both definitions. Test/alxIna228TestHelpers.c:112 and
    Test/alxPi4ioeTestHelpers.c:99 are the other two overrides in the suite.
    """
    ovr = mem_raw_ovr_lib
    me = ovr.new()
    assert ovr.count("Ctor") == 1, "the product's constructor did not displace the library's"
    assert ovr.asserts() == 0

    assert ovr.init(me) == ovr.OK
    assert ovr.read(me, addr=0, length=8) == ovr.OK
    assert ovr.write(me, addr=0, length=8) == ovr.OK
    assert (ovr.count("Init"), ovr.count("Read"), ovr.count("Write")) == (1, 1, 1)
    assert ovr.asserts() == 0, "one of the four displaced calls reached the library's stub"

    assert ovr.deinit(me) == ovr.OK, "the undisplaced stub returns Alx_Ok, as P517 seals"
    assert ovr.asserts() == 1, "AlxMemRaw_DeInit was displaced - it is meant to be left weak here"
    assert ovr.first() == ASSERT_TEXT["DeInit"]


# =====================================================================
# P521 - the three parameters the header never defines
# =====================================================================


def test_ALX1553_P521_the_callers_retry_count_and_timeout_arrive_at_the_override_unmodified(mem_raw_ovr_lib):
    """numOfTries, timeout_ms and checkWithReadEnable pass straight through, uninterpreted.

    Every @brief in both files is empty and the only stated contract is @retval Alx_Ok / Alx_Err, so
    what these three parameters mean is defined nowhere: not their units, not whether len may be 0,
    not any bound on addr + len, not whether Write is atomic, not what Read may leave in the buffer
    on failure. The struct carries no device geometry either - sizeof is 2, two bools - so the
    library has no means to bound anything, and every check that exists lives in the product's
    override.

    Which makes the one thing a test CAN pin worth pinning: the values a caller passes are the values
    the override is handed, unmodified and unsubstituted, and the library does not retry on the
    caller's behalf - one call in, one call out, whatever numOfTries says. The retry loop at
    alxMemSafe.c:150 is that module's OWN memSafeReadWriteNumOfTries, a different counter entirely,
    and an override author who assumed the library was counting for them would implement nothing.

    Worth noting beside it: alxMemSafe passes checkWithReadEnable = true at all five of its Write
    call sites (243, 279, 315, 407, 433), so the false branch of that flag is not exercised anywhere
    in the repository. It is exercised here. And Alx_Status already contains Alx_ErrNumOfTries, the
    natural return for a parameter called numOfTries, which the header's @retval list omits.
    """
    ovr = mem_raw_ovr_lib
    me = ovr.new()

    assert ovr.read(me, addr=16, length=4, tries=7, timeout_ms=1234) == ovr.OK
    assert ovr.count("Read") == 1, "the library retried the caller's read on its own"
    assert (ovr.last("Addr"), ovr.last("Len")) == (16, 4)
    assert (ovr.last("NumOfTries"), ovr.last("Timeout_ms")) == (7, 1234)

    assert ovr.write(me, addr=32, length=4, check=False, tries=2, timeout_ms=50) == ovr.OK
    assert ovr.count("Write") == 1
    assert (ovr.last("NumOfTries"), ovr.last("Timeout_ms")) == (2, 50)
    assert ovr.last("CheckWithReadEnable") is False, "the flag the whole repository only ever sets true"

    assert ovr.write(me, addr=32, length=4, check=True, tries=2, timeout_ms=50) == ovr.OK
    assert ovr.last("CheckWithReadEnable") is True


# =====================================================================
# P522 - the same caller, against a product and against the stub
# =====================================================================


@pytest.mark.expect_assert(
    "P522: the weak half of the comparison asserts twice, once per call - the point is that the "
    "caller cannot tell the difference from the status alone"
)
def test_ALX1553_P522_a_stored_parameter_survives_a_write_and_a_read_only_in_the_override(
        mem_raw_lib, mem_raw_ovr_lib):
    """Write eight bytes, read them back, and compare what each image tells a caller that checks status.

    Against the product's override this is what the contract is for: the bytes go to the device, the
    bytes come back, and the caller has its parameters. Against the library's weak defaults the
    identical caller code gets the identical two Alx_Ok statuses and nothing has moved - the device
    still reads as erased, and the buffer still holds whatever it held before.

    That equality of status is the finding P517 and P518 seal, stated as the thing a product would
    actually experience: a board whose parameters never persist, whose every call reports success,
    and which on the library's shipped configuration says nothing at all. Written here as a
    characterization because the two seals carry the defect; this one carries the consequence.
    """
    ovr = mem_raw_ovr_lib
    me = ovr.new()
    ovr.fill(0x00)
    for index, byte in enumerate(PARAMS):
        ovr.c.AlxMemRawTest_BuffPoke(index, byte)

    assert ovr.write(me, addr=0, length=len(PARAMS)) == ovr.OK
    assert ovr.mem(0, len(PARAMS)) == PARAMS, "the product's Write did not reach the device"

    ovr.fill(0xA5)
    assert ovr.read(me, addr=0, length=len(PARAMS)) == ovr.OK
    assert ovr.buff(len(PARAMS)) == PARAMS, "the product's Read did not bring the parameters back"

    weak = mem_raw_lib
    stub = weak.new()
    weak.fill(0xA5)
    assert weak.write(stub, addr=0, length=len(PARAMS)) == weak.OK == ovr.OK
    assert weak.read(stub, addr=0, length=len(PARAMS)) == weak.OK == ovr.OK
    assert weak.buff(len(PARAMS)) == bytes([0xA5] * len(PARAMS)), "the weak Read touched the buffer"
    assert weak.asserts() == 2


# =====================================================================
# P523 - every one of the five weak bodies, and which of them assert
# =====================================================================


@pytest.mark.expect_assert(
    "P523: four of the five weak defaults assert - counting them is the measurement, and the fifth "
    "not asserting is the finding"
)
def test_ALX1553_P523_the_constructor_is_the_one_weak_default_that_says_nothing(mem_raw_lib):
    """Five weak bodies driven in order, and the assertion count comes to four.

    The four functions announce that they must not be called. The constructor does not, and it is
    the call a product makes FIRST - so on a board whose override was forgotten, the first chance to
    notice is not construction but Init, and only in a build that defined an enable macro and
    supplied a strong AlxAssert_Rst. The constructor is also the one of the five that a product can
    most easily forget to override, because it has no return value to check.

    This is also the test the COVERAGE lane leans on. alxMemRaw.c is gated at 100 % of functions
    from the weak DLL, and a function no test calls is precisely what that gate exists to catch - an
    earlier draft of this file reached only the constructor and Read and the gate failed at 40 %.
    Driving all five here keeps that number honest rather than chosen.
    """
    me = mem_raw_lib.new()
    assert mem_raw_lib.asserts() == 0, "the constructor asserted"

    for call in ("Init", "DeInit", "Read", "Write"):
        _call(mem_raw_lib, me, call)

    assert mem_raw_lib.asserts() == 4, "one of the four stubs did not reach its ALX_MEM_RAW_ASSERT"
    assert mem_raw_lib.first() == ASSERT_TEXT["Init"], "the first record is not the first call"


# =====================================================================
# P524 - the two fields a product maintains for nobody
# =====================================================================


def test_ALX1553_P524_the_override_maintains_two_fields_no_library_function_reads(mem_raw_ovr_lib):
    """What a correct override does with wasCtorCalled and isInit, and what checks it: nothing.

    The override sets wasCtorCalled true and isInit false in the constructor and isInit true in
    Init, which is the convention alxMemSafe.c and alxCrc.c follow - and it does so on its own
    initiative, because the weak constructor writes neither (P516) and no function in the library
    reads either for this module. alxMemSafe opens six of its own public functions with
    ALX_MEM_SAFE_ASSERT(me->wasCtorCalled == true); nothing anywhere does that for an AlxMemRaw.

    So the two fields are real state that a product is silently responsible for and that nothing can
    check, which is the same shape as P521 one level down: the header declares the contract's
    vocabulary and defines none of its meaning. The header change that would settle it is small -
    either the weak constructor sets them and the public functions assert on them, or the fields
    come out.
    """
    ovr = mem_raw_ovr_lib
    me = ovr.new()

    assert (ovr.field(me, "WasCtorCalled"), ovr.field(me, "IsInit")) == (1, 0), (
        "the product's constructor did not write the two fields the library's never writes")

    assert ovr.init(me) == ovr.OK
    assert ovr.field(me, "IsInit") == 1, "the override's Init did not record that it had run"

    stale = ovr.new_no_ctor()
    assert ovr.read(stale, addr=0, length=4) == ovr.OK, (
        "a read on an object whose constructor never ran still succeeds - nothing checks the fields")
    assert ovr.asserts() == 0
