"""C library host test application: pytest lifecycle and policy hooks.

Build/access mechanisms are in host_harness.py; tests import that module directly.
"""

import os
from pathlib import Path

import pytest

from host_harness import (
    _LOADED_DLLS,
    ASSERT_DEPS,
    ASSERT_DLL,
    ASSERT_WEAK_DEPS,
    ASSERT_WEAK_DLL,
    AUDIO_DEPS,
    AUDIO_DLL,
    AUDIOPLAYER_DEPS,
    AUDIOPLAYER_DLL,
    BOOL_DEPS,
    BOOL_DLL,
    BTS_DEPS,
    BTS_DLL,
    CANPARSER_DEPS,
    CANPARSER_DLL,
    CLI_DEPS,
    CLI_DLL,
    DELAY_DEPS,
    DELAY_DLL,
    FIFO_DEPS,
    FIFO_DLL,
    FILTGLITCH_DEPS,
    FILTGLITCH_DLL,
    FSSAFE_DEPS,
    FSSAFE_DLL,
    ID_DEPS,
    ID_DLL,
    INA228_DEPS,
    INA228_DLL,
    LIN_DEPS,
    LIN_DLL,
    LINFUN_DEPS,
    LINFUN_DLL,
    MATH_DEPS,
    MATH_DLL,
    MEMRAW_DEPS,
    MEMRAW_DLL,
    MEMRAW_OVR_DEPS,
    MEMRAW_OVR_DLL,
    MEMSAFE_DEPS,
    MEMSAFE_DLL,
    MUX_DEPS,
    MUX_DLL,
    NTC_DEPS,
    NTC_DLL,
    PARAMKV_DEPS,
    PARAMKV_DLL,
    PARAMMGMT_DEPS,
    PARAMMGMT_DLL,
    PI4IOE_DEPS,
    PI4IOE_DLL,
    PWR_DEPS,
    PWR_DLL,
    ROTSW_DEPS,
    ROTSW_DLL,
    RTC_DEPS,
    RTC_DLL,
    TEMPSENS_DEPS,
    TEMPSENS_DLL,
    TIMSW_DEPS,
    TIMSW_DLL,
    VARIANTS,
    VDIV_DEPS,
    VDIV_DLL,
    AssertLib,
    AssertWeakLib,
    AudioLib,
    AudioPlayerLib,
    AudioVolLib,
    BoolLib,
    BoundLib,
    BtsLib,
    CanParserLib,
    CliLib,
    CliUnderTest,
    DelayLib,
    FiltGlitchLib,
    FsSafeLib,
    IdLib,
    Ina228Lib,
    Lib,
    LinFunLib,
    LinLib,
    MathLib,
    MemRawLib,
    MemRawOverrideLib,
    MemSafeLib,
    MuxLib,
    NtcLib,
    ParamKvStoreLib,
    ParamMgmtLib,
    Pi4ioeLib,
    PwrLib,
    RotSwLib,
    RtcLib,
    TempSensLib,
    TimSwLib,
    VdivLib,
    _assert_pins_fitted,
    _build_assert_dll,
    _build_assert_weak_dll,
    _build_audio_dll,
    _build_audioplayer_dll,
    _build_bool_dll,
    _build_bts_dll,
    _build_canparser_dll,
    _build_cli_dll,
    _build_delay_dll,
    _build_fifo_dll,
    _build_filtglitch_dll,
    _build_fssafe_dll,
    _build_id_dll,
    _build_ina228_dll,
    _build_lin_dll,
    _build_linfun_dll,
    _build_math_dll,
    _build_memraw_dll,
    _build_memraw_ovr_dll,
    _build_memsafe_dll,
    _build_mux_dll,
    _build_ntc_dll,
    _build_paramkv_dll,
    _build_parammgmt_dll,
    _build_pi4ioe_dll,
    _build_pwr_dll,
    _build_rotsw_dll,
    _build_rtc_dll,
    _build_tempsens_dll,
    _build_timsw_dll,
    _build_vdiv_dll,
    _needs_build,
    _variant_lib,
)

pytest_plugins = ("alx.verify.evidence",)


@pytest.fixture(autouse=True)
def _no_library_assertions(request):
    """No test may leave one of the library's own assertions tripped.

    Reset before, checked after, across every DLL loaded so far - which is what makes it work for
    the session-scoped wrappers too.

    A test that MEANS to trip one marks itself `@pytest.mark.expect_assert("why")`, and the reason
    is required: an exemption that did not have to say what it was for would be added silently the
    first time one got in the way, which is how 257 assertions came to be compiled out of the
    device repository's host build in the first place.
    """
    for c in _LOADED_DLLS:
        c.AlxAssertPc_Reset()
    yield
    marker = request.node.get_closest_marker("expect_assert")
    for c in _LOADED_DLLS:
        count = c.AlxAssertPc_Count()
        if count and marker is None:
            where = (c.AlxAssertPc_First() or b"").decode("ascii", "replace")
            msg = f"a library assertion failed {count} time(s): {where}"
            raise AssertionError(msg)
    if marker is not None:
        assert marker.args, "expect_assert has to say what the assertion is for"
        assert marker.args[0], "expect_assert's reason cannot be empty"


@pytest.fixture(scope="session")
def lib() -> Lib:
    # ALX_FIFO_TEST_DLL selects an externally built DLL variant (coverage/sanitizer
    # build) instead of the default clang build - same suite, instrumented binary.
    override = os.environ.get("ALX_FIFO_TEST_DLL")
    if override:
        return Lib(Path(override))
    if _needs_build(FIFO_DLL, FIFO_DEPS):
        _build_fifo_dll()
    return Lib(FIFO_DLL)


@pytest.fixture(scope="session", params=sorted(VARIANTS))
def variant_lib(request) -> tuple[str, Lib]:
    """The FIFO group built in ONE named configuration - what a variant test asks for.

    Session-scoped and parametrized, so every DLL is built once and a plain `pytest` run covers all
    four configurations for the behaviour that differs between them. The rest of the suite stays on
    `default`: a FIFO write and read behave identically in all four, and running 1400 tests four
    times would cost four times the wall clock to learn nothing.
    """
    return request.param, _variant_lib("fifo", request.param, Lib)


@pytest.fixture(scope="session")
def bound(lib) -> BoundLib:
    # same test-group DLL as alxFifo (alxBound.c is linked into it anyway)
    return BoundLib(lib.c)


@pytest.fixture(scope="session")
def cli_lib() -> CliLib:
    # ALX_CLI_TEST_DLL selects an externally built variant (coverage/sanitizer)
    override = os.environ.get("ALX_CLI_TEST_DLL")
    if override:
        return CliLib(Path(override))
    if _needs_build(CLI_DLL, CLI_DEPS):
        _build_cli_dll()
    return CliLib(CLI_DLL)


@pytest.fixture
def make_cli(cli_lib):
    """Factory: make_cli() -> CliUnderTest with a fresh CLI + fake port. Auto-deleted."""
    ctxs = []

    def _make() -> CliUnderTest:
        ctx = cli_lib.c.AlxCliTest_New()
        ctxs.append(ctx)
        return CliUnderTest(cli_lib, ctx)

    yield _make
    for ctx in ctxs:
        cli_lib.c.AlxCliTest_Delete(ctx)


@pytest.fixture
def make_fifo(lib):
    """Factory: make_fifo(buff_len, rotate=n) -> handle. Auto-deleted."""
    handles = []

    def _make(buff_len: int, rotate: int = 0):
        f = lib.new(buff_len)
        handles.append(f)
        if rotate:
            assert rotate < buff_len
            assert lib.write(f, b"x" * rotate) == lib.OK
            status, _ = lib.read(f, rotate)
            assert status == lib.OK
            assert lib.entries(f) == 0
        return f

    yield _make
    for h in handles:
        lib.delete(h)


@pytest.fixture(scope="session")
def timsw_lib() -> TimSwLib:
    override = os.environ.get("ALX_TIMSW_TEST_DLL")
    if override:
        return TimSwLib(Path(override))
    if _needs_build(TIMSW_DLL, TIMSW_DEPS):
        _build_timsw_dll()
    return TimSwLib(TIMSW_DLL)


@pytest.fixture
def clock(timsw_lib) -> TimSwLib:
    """The timer library with the clock back at zero and the fake's counters cleared."""
    timsw_lib.tick_reset()
    return timsw_lib


@pytest.fixture(scope="session")
def pi4ioe_lib_session() -> Pi4ioeLib:
    override = os.environ.get("ALX_PI4IOE_TEST_DLL")
    if override:
        return Pi4ioeLib(Path(override))
    if _needs_build(PI4IOE_DLL, PI4IOE_DEPS):
        _build_pi4ioe_dll()
    return Pi4ioeLib(PI4IOE_DLL)


@pytest.fixture
def pi4ioe_lib(pi4ioe_lib_session) -> Pi4ioeLib:
    """The expander library, with the previous test's devices released."""
    yield pi4ioe_lib_session
    pi4ioe_lib_session.free_all()


@pytest.fixture(scope="session")
def ina228_lib_session() -> Ina228Lib:
    override = os.environ.get("ALX_INA228_TEST_DLL")
    if override:
        return Ina228Lib(Path(override))
    if _needs_build(INA228_DLL, INA228_DEPS):
        _build_ina228_dll()
    return Ina228Lib(INA228_DLL)


@pytest.fixture
def ina228_lib(ina228_lib_session) -> Ina228Lib:
    """The sensor library, with the fake bus cleared and the previous test's sensors released."""
    yield ina228_lib_session
    ina228_lib_session.free_all()


@pytest.fixture(scope="session")
def param_mgmt_lib_session() -> ParamMgmtLib:
    override = os.environ.get("ALX_PARAMMGMT_TEST_DLL")
    if override:
        return ParamMgmtLib(Path(override))
    if _needs_build(PARAMMGMT_DLL, PARAMMGMT_DEPS):
        _build_parammgmt_dll()
    return ParamMgmtLib(PARAMMGMT_DLL)


@pytest.fixture
def param_mgmt_lib(param_mgmt_lib_session) -> ParamMgmtLib:
    """The manager library, with every table the previous test built already released."""
    yield param_mgmt_lib_session
    param_mgmt_lib_session.free_all()


@pytest.fixture(scope="session")
def rtc_lib() -> RtcLib:
    override = os.environ.get("ALX_RTC_TEST_DLL")
    if override:
        return RtcLib(Path(override))
    if _needs_build(RTC_DLL, RTC_DEPS):
        _build_rtc_dll()
    return RtcLib(RTC_DLL)


@pytest.fixture(scope="session")
def bool_lib_session() -> BoolLib:
    override = os.environ.get("ALX_BOOL_TEST_DLL")
    if override:
        return BoolLib(Path(override))
    if _needs_build(BOOL_DLL, BOOL_DEPS):
        _build_bool_dll()
    return BoolLib(BOOL_DLL)


@pytest.fixture
def bool_lib(bool_lib_session) -> BoolLib:
    """The boolean library with the clock back at zero and nothing left from the last test."""
    bool_lib_session.tick_reset()
    yield bool_lib_session
    bool_lib_session.free_all()


@pytest.fixture(scope="session")
def lin_fun_lib_session() -> LinFunLib:
    override = os.environ.get("ALX_LINFUN_TEST_DLL")
    if override:
        return LinFunLib(Path(override))
    if _needs_build(LINFUN_DLL, LINFUN_DEPS):
        _build_linfun_dll()
    return LinFunLib(LINFUN_DLL)


@pytest.fixture(scope="session")
def fs_safe_lib_session() -> FsSafeLib:
    override = os.environ.get("ALX_FSSAFE_TEST_DLL")
    if override:
        return FsSafeLib(Path(override))
    if _needs_build(FSSAFE_DLL, FSSAFE_DEPS):
        _build_fssafe_dll()
    return FsSafeLib(FSSAFE_DLL)


@pytest.fixture
def fs_safe_lib(fs_safe_lib_session) -> FsSafeLib:
    """Safe storage with an empty flash and no injected failures."""
    fs_safe_lib_session.c.AlxFsFake_Reset()
    yield fs_safe_lib_session
    fs_safe_lib_session.free_all()


@pytest.fixture(scope="session")
def lin_lib_session() -> LinLib:
    override = os.environ.get("ALX_LIN_TEST_DLL")
    if override:
        return LinLib(Path(override))
    if _needs_build(LIN_DLL, LIN_DEPS):
        _build_lin_dll()
    return LinLib(LIN_DLL)


@pytest.fixture(scope="session", params=sorted(VARIANTS))
def lin_variant_lib(request) -> tuple[str, LinLib]:
    """The LIN group built in ONE named configuration - the TRACE half of the matrix.

    alxFifo proved the assert axis and could not say anything about traces, because alxFifo.c and
    alxBound.c have no trace call sites at all. alxLin.c has 23, at three levels, and the
    INF/DBG boundary between them is the one the four variants actually cross.

    Deliberately NOT wired to ALX_LIN_TEST_DLL. A variant is a statement about what conftest's own
    recipe builds; honouring an externally built override here would let the sanitizer and coverage
    lanes silently answer a question about a different binary. See test_alxLin_variants.py.
    """
    return request.param, _variant_lib("lin", request.param, LinLib)


@pytest.fixture(scope="session", params=sorted(VARIANTS))
def memsafe_variant_lib(request) -> tuple[str, MemSafeLib]:
    """The MemSafe group in ONE named configuration - the trace axis on a group of 46 sites.

    Deliberately not wired to ALX_MEMSAFE_TEST_DLL, for the reason lin_variant_lib is not: a
    variant is a statement about what conftest's own recipe builds.
    """
    return request.param, _variant_lib("memsafe", request.param, MemSafeLib)


@pytest.fixture
def lin_variant(lin_variant_lib) -> tuple[str, LinLib]:
    """One named configuration of the LIN group, with this test's ports released afterwards.

    The session-scoped fixture builds each DLL once; this is what a test takes, because the serial
    fake has two port slots and running out of them is an exit(1) rather than a failure.
    """
    yield lin_variant_lib
    lin_variant_lib[1].free_all()


@pytest.fixture
def lin_lib(lin_lib_session) -> LinLib:
    """The LIN master, with the previous test's ports released."""
    yield lin_lib_session
    lin_lib_session.free_all()


@pytest.fixture(scope="session")
def delay_lib_session() -> DelayLib:
    override = os.environ.get("ALX_DELAY_TEST_DLL")
    if override:
        return DelayLib(Path(override))
    if _needs_build(DELAY_DLL, DELAY_DEPS):
        _build_delay_dll()
    return DelayLib(DELAY_DLL)


@pytest.fixture
def delay_lib(delay_lib_session) -> DelayLib:
    """The busy waits, with the clock back at zero and moving one nanosecond per read."""
    delay_lib_session.clock(step_ns=1)
    return delay_lib_session


@pytest.fixture(scope="session")
def param_kv_store_lib_session() -> ParamKvStoreLib:
    override = os.environ.get("ALX_PARAMKV_TEST_DLL")
    if override:
        return ParamKvStoreLib(Path(override))
    if _needs_build(PARAMKV_DLL, PARAMKV_DEPS):
        _build_paramkv_dll()
    return ParamKvStoreLib(PARAMKV_DLL)


@pytest.fixture
def param_kv_store_lib(param_kv_store_lib_session) -> ParamKvStoreLib:
    """The store with an empty, unmounted file system and no injected failures."""
    param_kv_store_lib_session.c.AlxFsFake_Reset()
    yield param_kv_store_lib_session
    param_kv_store_lib_session.free_all()


@pytest.fixture(scope="session")
def mux_lib_session() -> MuxLib:
    override = os.environ.get("ALX_MUX_TEST_DLL")
    if override:
        return MuxLib(Path(override))
    if _needs_build(MUX_DLL, MUX_DEPS):
        _build_mux_dll()
    return MuxLib(MUX_DLL)


@pytest.fixture
def mux_lib(mux_lib_session) -> MuxLib:
    """The multiplexer library with every pin back at its start-up level and count."""
    mux_lib_session.c.AlxIoPinFake_Reset()
    yield mux_lib_session
    _assert_pins_fitted(mux_lib_session)
    mux_lib_session.free_all()


@pytest.fixture(scope="session")
def bts_lib_session() -> BtsLib:
    override = os.environ.get("ALX_BTS_TEST_DLL")
    if override:
        return BtsLib(Path(override))
    if _needs_build(BTS_DLL, BTS_DEPS):
        _build_bts_dll()
    return BtsLib(BTS_DLL)


@pytest.fixture
def bts_lib(bts_lib_session) -> BtsLib:
    """The switch library with the clock back at zero and every pin low."""
    bts_lib_session.tick_reset()
    bts_lib_session.c.AlxIoPinFake_Reset()
    yield bts_lib_session
    _assert_pins_fitted(bts_lib_session)
    bts_lib_session.free_all()


@pytest.fixture(scope="session")
def ntc_lib() -> NtcLib:
    """The thermistor lookup; one free function, so one instance serves every test."""
    override = os.environ.get("ALX_NTC_TEST_DLL")
    if override:
        return NtcLib(Path(override))
    if _needs_build(NTC_DLL, NTC_DEPS):
        _build_ntc_dll()
    return NtcLib(NTC_DLL)


@pytest.fixture(scope="session")
def audio_player_lib_session() -> AudioPlayerLib:
    override = os.environ.get("ALX_AUDIOPLAYER_TEST_DLL")
    if override:
        return AudioPlayerLib(Path(override))
    if _needs_build(AUDIOPLAYER_DLL, AUDIOPLAYER_DEPS):
        _build_audioplayer_dll()
    return AudioPlayerLib(AUDIOPLAYER_DLL)


@pytest.fixture
def audio_player_lib(audio_player_lib_session) -> AudioPlayerLib:
    """The player library, with everything the previous test allocated already released."""
    yield audio_player_lib_session
    audio_player_lib_session.free_all()


@pytest.fixture(scope="session")
def audio_lib() -> AudioLib:
    """The audio conversions; nothing here holds state, so one instance serves every test."""
    override = os.environ.get("ALX_AUDIO_TEST_DLL")
    if override:
        return AudioLib(Path(override))
    if _needs_build(AUDIO_DLL, AUDIO_DEPS):
        _build_audio_dll()
    return AudioLib(AUDIO_DLL)


@pytest.fixture(scope="session")
def pwr_lib_session() -> PwrLib:
    override = os.environ.get("ALX_PWR_TEST_DLL")
    if override:
        return PwrLib(Path(override))
    if _needs_build(PWR_DLL, PWR_DEPS):
        _build_pwr_dll()
    return PwrLib(PWR_DLL)


@pytest.fixture
def pwr_lib(pwr_lib_session) -> PwrLib:
    """The supervisor library with the clock back at zero and nothing left from the last test."""
    pwr_lib_session.tick_reset()
    yield pwr_lib_session
    pwr_lib_session.free_all()


@pytest.fixture(scope="session")
def temp_sens_lib_session() -> TempSensLib:
    override = os.environ.get("ALX_TEMPSENS_TEST_DLL")
    if override:
        return TempSensLib(Path(override))
    if _needs_build(TEMPSENS_DLL, TEMPSENS_DEPS):
        _build_tempsens_dll()
    return TempSensLib(TEMPSENS_DLL)


@pytest.fixture
def temp_sens_lib(temp_sens_lib_session) -> TempSensLib:
    """The sensor library, with every converter channel back at zero."""
    temp_sens_lib_session.c.AlxAdcFake_Reset()
    yield temp_sens_lib_session
    temp_sens_lib_session.free_all()


@pytest.fixture(scope="session")
def rot_sw_lib_session() -> RotSwLib:
    override = os.environ.get("ALX_ROTSW_TEST_DLL")
    if override:
        return RotSwLib(Path(override))
    if _needs_build(ROTSW_DLL, ROTSW_DEPS):
        _build_rotsw_dll()
    return RotSwLib(ROTSW_DLL)


@pytest.fixture
def rot_sw_lib(rot_sw_lib_session) -> RotSwLib:
    """The rotary switch, with every pin back at zero and nothing left from the last test."""
    rot_sw_lib_session.c.AlxIoPinFake_Reset()
    yield rot_sw_lib_session
    _assert_pins_fitted(rot_sw_lib_session)
    rot_sw_lib_session.free_all()


@pytest.fixture(scope="session")
def audiovol_lib_session() -> AudioVolLib:
    override = os.environ.get("ALX_LINFUN_TEST_DLL")
    if override:
        return AudioVolLib(Path(override))
    if _needs_build(LINFUN_DLL, LINFUN_DEPS):
        _build_linfun_dll()
    return AudioVolLib(LINFUN_DLL)


@pytest.fixture
def audiovol_lib(audiovol_lib_session) -> AudioVolLib:
    """The audio volume, with everything the previous test allocated already released."""
    yield audiovol_lib_session
    audiovol_lib_session.free_all()


@pytest.fixture
def lin_fun_lib(lin_fun_lib_session) -> LinFunLib:
    """The mapping library, with everything the previous test allocated already released."""
    yield lin_fun_lib_session
    lin_fun_lib_session.free_all()


@pytest.fixture(scope="session")
def math_lib_session() -> MathLib:
    override = os.environ.get("ALX_MATH_TEST_DLL")
    if override:
        return MathLib(Path(override))
    if _needs_build(MATH_DLL, MATH_DEPS):
        _build_math_dll()
    return MathLib(MATH_DLL)


@pytest.fixture
def math_lib(math_lib_session) -> MathLib:
    """The arithmetic library, with everything the previous test allocated already released."""
    yield math_lib_session
    math_lib_session.free_all()


@pytest.fixture(scope="session")
def filt_glitch_lib() -> FiltGlitchLib:
    override = os.environ.get("ALX_FILTGLITCH_TEST_DLL")
    if override:
        return FiltGlitchLib(Path(override))
    if _needs_build(FILTGLITCH_DLL, FILTGLITCH_DEPS):
        _build_filtglitch_dll()
    return FiltGlitchLib(FILTGLITCH_DLL)


@pytest.fixture
def filt(filt_glitch_lib) -> FiltGlitchLib:
    """The filter library with the clock back at zero and nothing left over from the last test."""
    filt_glitch_lib.tick_reset()
    yield filt_glitch_lib
    filt_glitch_lib.free_all()


@pytest.fixture(scope="session")
def canparser_lib() -> CanParserLib:
    override = os.environ.get("ALX_CANPARSER_TEST_DLL")
    if override:
        return CanParserLib(Path(override))
    if _needs_build(CANPARSER_DLL, CANPARSER_DEPS):
        _build_canparser_dll()
    return CanParserLib(CANPARSER_DLL)


@pytest.fixture(scope="session")
def vdiv_lib() -> VdivLib:
    override = os.environ.get("ALX_VDIV_TEST_DLL")
    if override:
        return VdivLib(Path(override))
    if _needs_build(VDIV_DLL, VDIV_DEPS):
        _build_vdiv_dll()
    return VdivLib(VDIV_DLL)


@pytest.fixture(scope="session")
def memsafe_lib() -> MemSafeLib:
    # ALX_MEMSAFE_TEST_DLL selects an externally built variant (coverage/sanitizer)
    override = os.environ.get("ALX_MEMSAFE_TEST_DLL")
    if override:
        return MemSafeLib(Path(override))
    if _needs_build(MEMSAFE_DLL, MEMSAFE_DEPS):
        _build_memsafe_dll()
    return MemSafeLib(MEMSAFE_DLL)


@pytest.fixture
def flash(memsafe_lib) -> MemSafeLib:
    """The fake flash, blank (0xFF) and fault-free at the start of every test."""
    memsafe_lib.fake_reset()
    return memsafe_lib


@pytest.fixture
def make_item(memsafe_lib):
    """Factory: make_item(type, val_def, val_min, val_max) -> a standalone AlxParamItem.

    Outside the store chain on purpose: what these tests drive is the string format conversion
    that a CLI set-param reaches, not persistence. Auto-deleted.
    """
    ctxs = []

    def _make(item_type: int, val_def: float = 0.0, val_min: float = 0.0, val_max: float = 0.0):
        ctx = memsafe_lib.item_new(item_type, val_def, val_min, val_max)
        assert ctx, "the item could not be constructed"
        ctxs.append(ctx)
        return ctx

    yield _make
    for ctx in ctxs:
        memsafe_lib.item_delete(ctx)


@pytest.fixture
def make_meta_item(memsafe_lib):
    """Factory: make_meta_item(data_type, **ctor kwargs) -> an item, auto-deleted.

    The item stands alone, exactly as make_item's does: no kv store and no group object, because
    what is under test is what the CONSTRUCTOR recorded, not what a store would do with it.
    """
    ctxs = []

    def _make(data_type: int, **kwargs):
        ctx = memsafe_lib.meta_new(data_type, **kwargs)
        assert ctx, f"the item of type {data_type} could not be constructed"
        ctxs.append(ctx)
        return ctx

    yield _make
    for ctx in ctxs:
        memsafe_lib.meta_delete(ctx)


@pytest.fixture
def make_enum_item(memsafe_lib):
    """Factory: make_enum_item(data_type, values, ...) -> an item with an enum list, auto-deleted."""
    ctxs = []

    def _make(data_type: int, values, **kwargs):
        ctx = memsafe_lib.enum_new(data_type, values, **kwargs)
        assert ctx, f"the enum item of type {data_type} could not be constructed"
        ctxs.append(ctx)
        return ctx

    yield _make
    for ctx in ctxs:
        memsafe_lib.enum_delete(ctx)


@pytest.fixture
def make_buff_item(memsafe_lib):
    """Factory: make_buff_item("arr"|"str", ...) -> an item whose storage the context owns."""
    ctxs = []

    def _make(kind: str, *args, **kwargs):
        ctx = (memsafe_lib.buff_new_arr(*args, **kwargs) if kind == "arr"
               else memsafe_lib.buff_new_str(*args, **kwargs))
        assert ctx, f"the {kind} item could not be constructed"
        ctxs.append(ctx)
        return ctx

    yield _make
    for ctx in ctxs:
        memsafe_lib.buff_delete(ctx)


@pytest.fixture(params=list(VARIANTS))
def cli_variant_lib(request) -> tuple[str, CliLib]:
    """The CLI group built in each named configuration. Not wired to ALX_CLI_TEST_DLL, for
    the reason the other variant fixtures are not: a lane that overrides the DLL would hand
    every configuration the same file and the test would pass without measuring anything."""
    return request.param, _variant_lib("cli", request.param, CliLib)


@pytest.fixture
def make_cli_variant(cli_variant_lib):
    """Factory over a variant CLI: make_cli_variant() -> CliUnderTest, auto-deleted."""
    _name, lib = cli_variant_lib
    ctxs = []

    def _make() -> CliUnderTest:
        ctx = lib.c.AlxCliTest_New()
        ctxs.append(ctx)
        return CliUnderTest(lib, ctx)

    yield _make
    for ctx in ctxs:
        lib.c.AlxCliTest_Delete(ctx)


@pytest.fixture
def make_multi_store(memsafe_lib):
    """Factory: make_multi_store(groups) -> a store of that many groups, auto-deleted.

    The fake flash is global, so the fixture clears it first: two stores in one test would
    otherwise read each other's records.
    """
    memsafe_lib.fake_reset()
    ctxs = []

    def _make(groups: int):
        ctx = memsafe_lib.store_new(groups)
        assert ctx, f"a store of {groups} group(s) could not be constructed"
        ctxs.append(ctx)
        return ctx

    yield _make
    for ctx in ctxs:
        memsafe_lib.store_delete(ctx)


@pytest.fixture
def make_kv_item(memsafe_lib):
    """Factory: make_kv_item(data_type, key, val_def) -> an item WITH a store, auto-deleted.

    Arms the fake on the way in and DISARMS it on the way out. Disarmed is the state every other
    group depends on - their items are built without a store, and a call arriving at the fake means
    a test wandered into persistence, which must keep failing loudly.
    """
    memsafe_lib.kv_reset()
    memsafe_lib.kv_enable(True)
    ctxs = []

    def _make(data_type: int, key: str = "KV_KEY", val_def: int = 0):
        ctx = memsafe_lib.kv_item_new(data_type, key, val_def)
        assert ctx, f"an item of type {data_type} with a store could not be constructed"
        ctxs.append(ctx)
        return ctx

    yield _make
    for ctx in ctxs:
        memsafe_lib.kv_item_delete(ctx)
    memsafe_lib.kv_reset()


@pytest.fixture
def make_store(flash):
    """Factory: make_store(addr_a, addr_b, tries, raw_tries) -> ctx over the shared fake flash.
    A second ctx over the same flash = a reboot. Auto-deleted."""
    ctxs = []

    def _make(addr_a: int = 0x000, addr_b: int = 0x100, tries: int = 3, raw_tries: int = 3):
        ctx = flash.new(addr_a, addr_b, tries, raw_tries)
        ctxs.append(ctx)
        return ctx

    yield _make
    for ctx in ctxs:
        flash.delete(ctx)


@pytest.fixture(scope="session")
def id_lib_session() -> IdLib:
    override = os.environ.get("ALX_ID_TEST_DLL")
    if override:
        return IdLib(Path(override))
    if _needs_build(ID_DLL, ID_DEPS):
        _build_id_dll()
    return IdLib(ID_DLL)


@pytest.fixture
def id_lib(id_lib_session) -> IdLib:
    """The identity library, with every strap back to floating and the last test's page released."""
    id_lib_session.c.AlxIoPinFake_Reset()
    yield id_lib_session
    _assert_pins_fitted(id_lib_session)
    id_lib_session.free_all()


@pytest.fixture(scope="session")
def assert_weak_lib_session() -> AssertWeakLib:
    override = os.environ.get("ALX_ASSERT_WEAK_TEST_DLL")
    if override:
        return AssertWeakLib(Path(override))
    if _needs_build(ASSERT_WEAK_DLL, ASSERT_WEAK_DEPS):
        _build_assert_weak_dll()
    return AssertWeakLib(ASSERT_WEAK_DLL)


@pytest.fixture
def assert_weak_lib(assert_weak_lib_session) -> AssertWeakLib:
    """The library's own handlers, with the trace recorder cleared."""
    assert_weak_lib_session.reset()
    return assert_weak_lib_session


@pytest.fixture(scope="session")
def assert_lib_session() -> AssertLib:
    override = os.environ.get("ALX_ASSERT_TEST_DLL")
    if override:
        return AssertLib(Path(override))
    if _needs_build(ASSERT_DLL, ASSERT_DEPS):
        _build_assert_dll()
    return AssertLib(ASSERT_DLL)


@pytest.fixture
def assert_lib(assert_lib_session) -> AssertLib:
    """The displaced handlers, with both recorders cleared."""
    assert_lib_session.reset()
    assert_lib_session.assert_reset()
    return assert_lib_session


@pytest.fixture(scope="session")
def mem_raw_lib_session() -> MemRawLib:
    override = os.environ.get("ALX_MEMRAW_TEST_DLL")
    if override:
        return MemRawLib(Path(override))
    if _needs_build(MEMRAW_DLL, MEMRAW_DEPS):
        _build_memraw_dll()
    return MemRawLib(MEMRAW_DLL)


@pytest.fixture
def mem_raw_lib(mem_raw_lib_session) -> MemRawLib:
    """The weak defaults, with the data buffer poisoned and every handle released after."""
    mem_raw_lib_session.fill(mem_raw_lib_session.POISON)
    yield mem_raw_lib_session
    mem_raw_lib_session.free_all()


@pytest.fixture(scope="session")
def mem_raw_ovr_lib_session() -> MemRawOverrideLib:
    override = os.environ.get("ALX_MEMRAW_OVR_TEST_DLL")
    if override:
        return MemRawOverrideLib(Path(override))
    if _needs_build(MEMRAW_OVR_DLL, MEMRAW_OVR_DEPS):
        _build_memraw_ovr_dll()
    return MemRawOverrideLib(MEMRAW_OVR_DLL)


@pytest.fixture
def mem_raw_ovr_lib(mem_raw_ovr_lib_session) -> MemRawOverrideLib:
    """A product's override over a blank device, with the data buffer poisoned."""
    mem_raw_ovr_lib_session.ovr_reset()
    mem_raw_ovr_lib_session.fill(mem_raw_ovr_lib_session.POISON)
    yield mem_raw_ovr_lib_session
    mem_raw_ovr_lib_session.free_all()
