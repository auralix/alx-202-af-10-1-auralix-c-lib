"""The library's fakes, proven on their own: every fake in host/fakes/ linked into one DLL.

A consumer links these files in place of the modules they fake, so each fake's controls are part of
what the library promises. Most fakes are also exercised by the group of a module that calls the
faked one; the ones written for consumers are linked by no other group, and this file is the only
proof they do what their controls say.

Proofs (ALX-1564):
  P306 ADC: a sequence is read in order and its last value repeats; a single value replaces it
  P307 CAN: a queued frame is read once, an empty queue answers FIFO empty, every poll is counted
  P308 CAN: a transmitted frame is recorded per bus and cleared on demand; the error flag is set
  P309 PWM: each channel remembers its last duty cycle and counts the writes
  P310 INA228: each setter is answered by its getter, per sensor, and an unset sensor reads zero
  P311 PI4IOE5V6534Q: one pin grid behind the controls and the driver API; a reused slot is low
  P312 LP586x: presence decides Init, every entry point is counted, a write lands in the instance
  P313 ID: the constructor's own strings and hardware fields, and the hardware ID a test sets
  P314 IO pin: the fake says how many pins it tells apart and raises its flag one pin later
  P315 delay and watchdog: the calls a consumer counts are counted, and a reset clears them
  P316 safe memory: blank reads as both copies bad, a write reads back from copy A
  P317 key-value store: constructor and Init fill the object, and a product-sized table fits
  P318 the portable entry points of the constructor-only fakes succeed
"""

import ctypes

import pytest

from harness.access import CanMsg


def test_ALX1564_P306_an_adc_sequence_is_read_in_order_and_its_last_value_repeats(fakes_lib):
    c = fakes_lib.c
    adc = fakes_lib.handle()
    assert c.AlxAdc_GetVoltage_V(adc, 3) == 0.0, "a channel nobody set reads 0 V"

    seq = (ctypes.c_float * 3)(1.0, 2.0, 3.0)
    c.AlxAdcFake_SetVoltageSeq_V(adc, 3, seq, 3)
    assert [c.AlxAdc_GetVoltage_V(adc, 3) for _ in range(5)] == [1.0, 2.0, 3.0, 3.0, 3.0]
    assert c.AlxAdcFake_ReadCount(adc, 3) == 6

    too_long = (ctypes.c_float * 17)(*([9.0] * 17))
    c.AlxAdcFake_SetVoltageSeq_V(adc, 3, too_long, 17)
    assert c.AlxAdc_GetVoltage_V(adc, 3) == 3.0, "a sequence longer than the fake holds is refused"

    c.AlxAdcFake_SetVoltage_V(adc, 3, 1.5)
    assert c.AlxAdc_GetVoltage_V(adc, 3) == 1.5, "a single value replaces the sequence"
    assert c.AlxAdc_GetVoltage_mV(adc, 3) == 1500
    assert c.AlxAdc_GetVoltage_V(fakes_lib.handle(), 3) == 0.0, "another converter is another slot"

    assert c.AlxAdc_Init(adc) == fakes_lib.OK
    assert c.AlxAdc_DeInit(adc) == fakes_lib.OK
    assert (c.AlxAdcFake_InitCount(), c.AlxAdcFake_DeInitCount()) == (1, 1)


def test_ALX1564_P307_a_queued_can_frame_is_read_once_and_every_poll_is_counted(fakes_lib):
    c = fakes_lib.c
    bus = fakes_lib.handle()
    data = (ctypes.c_uint8 * 8)(*range(8))
    assert c.AlxCanFake_QueueRxMsg(bus, 0x123, False, 8, data)

    msg = CanMsg()
    assert c.AlxCan_RxMsg(bus, ctypes.byref(msg)) == fakes_lib.OK
    got = (msg.id, msg.isExtendedId, msg.isDataFrame, msg.dataLen, list(msg.data))
    assert got == (0x123, False, True, 8, list(range(8)))
    assert c.AlxCan_RxMsg(bus, ctypes.byref(msg)) == fakes_lib.FIFO_ERR_EMPTY, "a frame is read once"
    assert c.AlxCanFake_RxMsgCount(bus) == 2, "every poll counts, whether a frame was there or not"
    assert c.AlxCanFake_RxMsgCount(fakes_lib.handle()) == 0, "buses are counted apart"


def test_ALX1564_P308_a_transmitted_can_frame_is_recorded_per_bus(fakes_lib):
    c = fakes_lib.c
    bus, other = fakes_lib.handle(), fakes_lib.handle()
    msg = CanMsg(id=0x1ABCDEF, isExtendedId=True, isDataFrame=True, dataLen=2)
    msg.data[0], msg.data[1] = 0xAA, 0x55
    assert c.AlxCan_TxMsg(bus, msg) == fakes_lib.OK
    assert (c.AlxCanFake_TxCount(bus), c.AlxCanFake_TxCount(other)) == (1, 0)

    msg_id, ext, dlc, data = ctypes.c_uint32(), ctypes.c_bool(), ctypes.c_uint8(), (ctypes.c_uint8 * 8)()
    args = (ctypes.byref(msg_id), ctypes.byref(ext), ctypes.byref(dlc), data)
    assert c.AlxCanFake_TxMsg(bus, 0, *args)
    assert (msg_id.value, ext.value, dlc.value, data[0], data[1]) == (0x1ABCDEF, True, 2, 0xAA, 0x55)
    assert not c.AlxCanFake_TxMsg(bus, 1, *args), "there is no second frame"

    c.AlxCanFake_ClearTx(bus)
    assert c.AlxCanFake_TxCount(bus) == 0

    assert not c.AlxCan_IsErr(bus)
    c.AlxCanFake_SetErr(bus, True)
    assert c.AlxCan_IsErr(bus)
    assert not c.AlxCan_IsErr(other), "the error flag is per bus"


def test_ALX1564_P309_a_pwm_channel_remembers_its_last_duty_and_counts_the_writes(fakes_lib):
    c = fakes_lib.c
    pwm = fakes_lib.handle()
    assert c.AlxPwmFake_Duty_pct(pwm, 1) == 0.0
    assert c.AlxPwm_SetDuty_pct(pwm, 1, 25.0) == fakes_lib.OK
    assert c.AlxPwm_SetDuty_pct(pwm, 1, 75.5) == fakes_lib.OK
    assert c.AlxPwmFake_Duty_pct(pwm, 1) == pytest.approx(75.5)
    assert c.AlxPwmFake_WriteCount(pwm, 1) == 2
    assert c.AlxPwmFake_Duty_pct(pwm, 2) == 0.0, "channels are apart"
    assert c.AlxPwmFake_WriteCount(fakes_lib.handle(), 1) == 0, "and so are timers"


@pytest.mark.parametrize(("what", "value"), [
    ("Current_A", 1.25), ("BusVoltage_V", 24.0), ("ShuntVoltage_V", 0.004), ("Power_W", 30.0),
    ("Temp_degC", 41.5),
])
def test_ALX1564_P310_each_ina228_setter_is_answered_by_its_getter(fakes_lib, what, value):
    c = fakes_lib.c
    ina, unset = fakes_lib.handle(), fakes_lib.handle()
    getattr(c, f"AlxIna228Fake_Set{what}")(ina, value)
    out = ctypes.c_float(-1.0)
    assert getattr(c, f"AlxIna228_Get{what}")(ina, ctypes.byref(out)) == fakes_lib.OK
    assert out.value == pytest.approx(value)
    assert getattr(c, f"AlxIna228_Get{what}")(unset, ctypes.byref(out)) == fakes_lib.OK
    assert out.value == 0.0, "a sensor nobody set reads zero"


def test_ALX1564_P311_one_expander_pin_grid_behind_the_controls_and_the_driver(fakes_lib):
    c = fakes_lib.c
    exp = fakes_lib.handle()
    c.AlxPi4ioe5v6534qFake_SetLevel(exp, 2, 5, True)
    assert c.AlxPi4ioe5v6534q_IoPin_Read(exp, 2, 5), "the driver reads what the test drove"
    c.AlxPi4ioe5v6534q_IoPin_Write(exp, 1, 0, True)
    assert c.AlxPi4ioe5v6534qFake_Level(exp, 1, 0), "the test reads what the driver wrote"
    assert not c.AlxPi4ioe5v6534qFake_Level(exp, 8, 0), "a port the fake does not have reads low"
    c.AlxPi4ioe5v6534qFake_SetLevel(exp, 0, 8, True)
    assert not c.AlxPi4ioe5v6534qFake_Level(exp, 0, 8), "and a pin it does not have is not kept"

    c.AlxPi4ioe5v6534qFake_Reset()
    other = fakes_lib.handle()
    assert not c.AlxPi4ioe5v6534qFake_Level(other, 2, 5), "a slot reused after a reset starts low"


def test_ALX1564_P312_led_driver_presence_decides_init_and_every_call_is_counted(fakes_lib):
    c = fakes_lib.c
    led = fakes_lib.new("Lp586x")
    assert c.AlxLp586x_Init(led) == fakes_lib.OK, "present by default"
    c.AlxLp586xFake_SetPresent(False)
    assert c.AlxLp586x_Init(led) == fakes_lib.ERR, "absent, Init says so"

    assert c.AlxLp586x_InitPeriph(led) == fakes_lib.OK
    assert c.AlxLp586x_DeInitPeriph(led) == fakes_lib.OK
    assert c.AlxLp586x_Handle(led) == fakes_lib.OK
    c.AlxLp586x_Led_Write(led, 7, True)
    c.AlxLp586x_Led_Write(led, 200, True)
    assert c.AlxFakesTest_Lp586x_ValNew(led, 7), "the requested value lands in the instance"
    counts = [getattr(c, f"AlxLp586xFake_{what}Count")()
              for what in ("InitPeriph", "DeInitPeriph", "Init", "Handle", "LedWrite")]
    assert counts == [1, 1, 2, 1, 2], "a write to a LED the chip does not have is still a call"

    c.AlxLp586xFake_Reset()
    assert c.AlxLp586xFake_LedWriteCount() == 0
    assert c.AlxLp586x_Init(led) == fakes_lib.OK, "a reset makes the chip present again"


def test_ALX1564_P313_id_answers_its_constructor_and_the_hardware_id_a_test_sets(fakes_lib):
    c = fakes_lib.c
    me = fakes_lib.new("Id")
    artf, name = ctypes.create_string_buffer(b"ARTF-1"), ctypes.create_string_buffer(b"Name One")
    c.AlxFakesTest_Id_Ctor(me, artf, name)
    assert c.AlxId_GetFwArtf(me) == b"ARTF-1"
    assert c.AlxId_GetFwName(me) == b"Name One"
    assert c.AlxFakesTest_Id_HwIsTheConstructorsOwn(me), "the fields the real constructor fills"

    assert c.AlxId_GetHwId(me) == 0
    c.AlxIdFake_SetHwId(7)
    assert c.AlxId_GetHwId(me) == 7

    # What the silicon would supply is neutral, so a test asserts the shape around it, never it.
    assert c.AlxId_GetFwVerStr(me) == b"0.0.0"
    assert c.AlxId_GetFwBinStr(me) == b"0000000000_HOST_0-0-0_0000000.bin"
    assert c.AlxId_GetHwMcuUniqueIdStr(me) == b"0" * 24

    c.AlxIdFake_Reset()
    assert (c.AlxId_GetFwArtf(me), c.AlxId_GetHwId(me)) == (b"", 0)


def test_ALX1564_P314_the_pin_fake_says_how_many_pins_it_tells_apart(fakes_lib):
    c = fakes_lib.c
    slots = c.AlxIoPinFake_NumOfSlots()
    pins = [fakes_lib.handle() for _ in range(slots)]
    for pin in pins:
        c.AlxIoPinFake_SetLevel(pin, True)
    assert not c.AlxIoPinFake_DidOverflow(), "every slot holds a pin of its own"
    c.AlxIoPinFake_SetLevel(fakes_lib.handle(), False)
    assert c.AlxIoPinFake_DidOverflow(), "one pin more than the slots raises the flag"
    c.AlxIoPinFake_Reset()
    assert not c.AlxIoPinFake_DidOverflow(), "a reset clears it"

    pin = fakes_lib.handle()
    c.AlxIoPin_Toggle(pin)
    assert (c.AlxIoPinFake_Level(pin), c.AlxIoPinFake_WriteCount(pin)) == (True, 1)
    c.AlxIoPin_Toggle(pin)
    assert (c.AlxIoPinFake_Level(pin), c.AlxIoPinFake_WriteCount(pin)) == (False, 2)


def test_ALX1564_P315_delays_and_watchdog_refreshes_are_counted(fakes_lib):
    c = fakes_lib.c
    c.AlxDelay_ms(10)
    c.AlxDelay_ms(10)
    c.AlxDelay_us(5)
    assert (c.AlxDelayFake_MsCount(), c.AlxDelayFake_UsCount()) == (2, 1)

    wdt = fakes_lib.handle()
    assert c.AlxWdt_Init(wdt) == fakes_lib.OK
    for _ in range(3):
        assert c.AlxWdt_Refresh(wdt) == fakes_lib.OK
    assert c.AlxWdtFake_RefreshCount() == 3

    c.AlxDelayFake_Reset()
    c.AlxWdtFake_Reset()
    assert (c.AlxDelayFake_MsCount(), c.AlxDelayFake_UsCount(), c.AlxWdtFake_RefreshCount()) == (0, 0, 0)


def test_ALX1564_P316_safe_memory_is_blank_until_written_and_reads_back_from_copy_a(fakes_lib):
    c = fakes_lib.c
    mem = fakes_lib.handle()
    buff = ctypes.create_string_buffer(4)
    assert c.AlxMemSafe_Read(mem, buff, 4) == fakes_lib.SAFE_BOTH_COPY_ERR, "blank memory"

    assert c.AlxMemSafe_Write(mem, b"\x01\x02\x03\x04", 4) == fakes_lib.OK
    assert c.AlxMemSafe_IsWriteDone(mem)
    assert not c.AlxMemSafe_IsWriteErr(mem)
    assert c.AlxMemSafe_Read(mem, buff, 4) == fakes_lib.SAFE_USE_COPY_A
    assert buff.raw == b"\x01\x02\x03\x04"

    longer = ctypes.create_string_buffer(5)
    assert c.AlxMemSafe_Read(mem, longer, 5) == fakes_lib.SAFE_BOTH_COPY_ERR, "more than was written"
    too_big = ctypes.create_string_buffer(8193)
    assert c.AlxMemSafe_Write(mem, too_big, 8193) == fakes_lib.ERR, "more than the memory holds"
    assert c.AlxMemSafe_Read(fakes_lib.handle(), buff, 4) == fakes_lib.SAFE_USE_COPY_A, (
        "the memory outlives the object: another handle reads the same block")

    c.AlxMemSafeFake_Reset()
    assert c.AlxMemSafe_Read(mem, buff, 4) == fakes_lib.SAFE_BOTH_COPY_ERR, "a reset blanks it"


def test_ALX1564_P317_the_key_value_store_holds_a_product_sized_table(fakes_lib):
    c = fakes_lib.c
    kv, fs = fakes_lib.new("ParamKvStore"), fakes_lib.handle()
    c.AlxParamKvStore_Ctor(kv, fs)
    assert c.AlxFakesTest_ParamKvStore_WasCtorCalled(kv)
    assert not c.AlxFakesTest_ParamKvStore_IsInit(kv)
    assert c.AlxFakesTest_ParamKvStore_Fs(kv) == fs
    assert c.AlxParamKvStore_Init(kv) == fakes_lib.OK
    assert c.AlxFakesTest_ParamKvStore_IsInit(kv)

    value = bytes(range(128))
    assert c.AlxParamKvStore_Set(kv, b"key", value, 128) == fakes_lib.ERR, "disarmed, it refuses"
    c.AlxParamKvStoreFake_Enable(True)

    key = b"K" * 63
    assert c.AlxParamKvStore_Set(kv, key, value, 128) == fakes_lib.OK
    out, length = ctypes.create_string_buffer(128), ctypes.c_uint32()
    assert c.AlxParamKvStore_Get(kv, key, out, 128, ctypes.byref(length)) == fakes_lib.OK
    assert (out.raw, length.value) == (value, 128), "a 128-byte value under a 63-character key"
    assert c.AlxParamKvStore_Set(kv, b"long", bytes(129), 129) == fakes_lib.ERR

    for i in range(1, 256):
        assert c.AlxParamKvStore_Set(kv, f"key{i}".encode(), b"v", 1) == fakes_lib.OK
    assert c.AlxParamKvStoreFake_NumOfKeys() == 256
    assert c.AlxParamKvStore_Set(kv, b"one-too-many", b"v", 1) == fakes_lib.ERR, "the table is full"
    assert c.AlxParamKvStore_Get(kv, b"never", out, 128, ctypes.byref(length)) == fakes_lib.ERR, (
        "a key never stored is the caller's first-boot path")


def test_ALX1564_P318_the_portable_entry_points_of_the_constructor_only_fakes_succeed(fakes_lib):
    c = fakes_lib.c
    me = fakes_lib.handle()
    assert c.AlxClk_Init(me) == fakes_lib.OK
    assert c.AlxRst_Init(me) == fakes_lib.OK
    c.AlxRst_Trace(me)
    c.AlxUsb_Irq_Handle(me)
    c.AlxBoot_Ctor(me, None, None, None, 100, 100)
    c.AlxBoot_App_Usb_Update(me)
    c.AlxFs_Ctor(me, 0, None, None, None, None, None, None)
    c.AlxTmp1075_Ctor(me, None, 0x48, False, 3, 10)
