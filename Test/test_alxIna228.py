"""ALX-1553 - alxIna228 PC unit tests (Tier 2 over a faked I2C bus: the current sensor's arithmetic).

The driver behind this product's main power measurement. What it does is turn register counts into
volts and amps, and the factors it uses are derived from two constructor arguments - the ADC range
and the shunt resistance. Those two numbers decide what the board's current reading can possibly
resolve, and nothing on a bench can separate them from the analog front end. Here they can be read
directly.

The part on this board is an **INA238**, not an INA228: `Usr/alxConfig.h` defines `ALX_INA238` and
comments `ALX_INA228` out, and one driver serves both. The difference matters more than the name -
the 238 divides its maximum expected current by 2^15 where the 228 divides by 2^19, so the same
board built the other way would report currents 16 times finer. The DLL is built with `ALX_INA238`.

The headline number, measured here and asserted below: with the 40.96 mV ADC range and the board's
100 uOhm shunt, **one count of the CURRENT register is 12.5 mA**. That is the floor on the raw
reading's resolution, and it is set by the driver deriving its maximum expected current from the FULL
ADC RANGE (409.6 A) rather than from anything the board will ever draw. See P226 and the task notes
for what that means for ALX-1480.

Writing these found one defect, sealed as P227: the calibration register is computed as a float that
is mathematically an exact 4096 for EVERY shunt, and then truncated to an integer - so for the shunt
values whose float lands a hair below, it is written as 4095. This board's shunt is not one of them.

One thing this suite cannot test is recorded rather than skipped silently: P225, the die temperature,
whose register overlay is not ABI-portable.

Test group P219-P227 = ALX-1553 current sensor proofs.
"""

import pytest

pytestmark = pytest.mark.unit

# the main power sensor of the product this suite was written for
BOARD_SHUNT_OHM = 0.0001
BOARD_SHUNT_PPM = 200.0

REG_VSHUNT, REG_VBUS, REG_DIETEMP, REG_CURRENT = 0x04, 0x05, 0x06, 0x07


@pytest.fixture
def board(ina228_lib):
    """The sensor configured exactly as this product's main power sensor is."""
    return ina228_lib.sensor(ina228_lib.RANGE_40_96_MV, BOARD_SHUNT_OHM, BOARD_SHUNT_PPM)


# =====================================================================
# P219-P220 - the two factors a board's accuracy is decided by
# =====================================================================


def test_ALX1553_P219_this_boards_current_reading_resolves_to_twelve_and_a_half_milliamps(
    ina228_lib, board, record_property
):
    """The measured consequence of the board's ADC range and shunt: 12.5 mA per count.

    Derivation, all of it in the driver: the maximum expected current is the ADC range divided by
    the shunt, 40.96 mV / 100 uOhm = 409.6 A; the current LSB is that divided by 2^15 for an INA238,
    which is 12.5 mA; and the calibration register is 819.2e6 x LSB x shunt x 4, which comes out at
    4096 - the part's own reset value. That last number is exact for THIS shunt; P227 shows the
    shunts for which the same arithmetic loses a count on its way to an integer.

    The 409.6 A is the whole ADC range, not an expectation about the board. Nothing in the driver
    takes a maximum expected current as an argument, so every product using it gets the coarsest LSB
    its shunt allows, whatever it actually draws.
    """
    lsb = ina228_lib.current_lsb(board)
    record_property("ina238_current_lsb_a", f"{lsb:.6f}")
    record_property("ina238_shunt_cal", ina228_lib.shunt_cal(board))
    assert lsb == pytest.approx(0.0125), "one count of the CURRENT register, in amps"
    assert ina228_lib.shunt_cal(board) == 4096


@pytest.mark.parametrize(("adc_range", "shunt_ohm", "lsb_a"), [
    ("RANGE_40_96_MV", 0.0001, 0.0125),        # this board
    ("RANGE_163_84_MV", 0.0001, 0.05),         # the same shunt on the wide range: four times coarser
    ("RANGE_40_96_MV", 0.001, 0.00125),        # a ten times larger shunt: ten times finer
    ("RANGE_163_84_MV", 0.05, 0.0001),         # a 50 mOhm shunt, a low current design
])
def test_ALX1553_P220_the_current_lsb_follows_the_range_and_the_shunt(
    ina228_lib, adc_range, shunt_ohm, lsb_a
):
    """Proportional to the ADC range, inversely proportional to the shunt, in four configurations.

    Worth having as its own test because it is the only lever a board has: the driver takes no
    maximum expected current, so resolution is decided entirely by these two numbers at design time.
    """
    sensor = ina228_lib.sensor(getattr(ina228_lib, adc_range), shunt_ohm)
    assert ina228_lib.current_lsb(sensor) == pytest.approx(lsb_a, rel=1e-6)


# =====================================================================
# P221-P223 - the readings
# =====================================================================


@pytest.mark.parametrize(("counts", "amps"), [
    (0, 0.0),
    (1, 0.0125),
    (2, 0.025),          # what this board reported at the bench's idle draw
    (15, 0.1875),
    (-1, -0.0125),       # the register is signed: current can flow either way
    (-2, -0.025),
    (32767, 409.5875),   # full scale, one count short of 409.6 A
    (-32768, -409.6),
])
def test_ALX1553_P221_the_current_register_is_counts_of_the_lsb_signed(
    ina228_lib, board, counts, amps
):
    """Every reading is the register times 12.5 mA, negatives included, to both ends of the range."""
    ina228_lib.set_register(REG_CURRENT, counts, 2)
    status, value = ina228_lib.read(board, "current_A")
    assert status == ina228_lib.OK
    assert value == pytest.approx(amps, rel=1e-5, abs=1e-9)


@pytest.mark.parametrize(("counts", "volts"), [(0, 0.0), (1, 0.003125), (7680, 24.0),
                                               (32767, 102.396875)])
def test_ALX1553_P222_the_bus_voltage_is_counts_of_3_125_millivolts(ina228_lib, board, counts,
                                                                    volts):
    """3.125 mV per count on an INA238 - and 24 V, the bench supply, is exactly 7680 counts."""
    ina228_lib.set_register(REG_VBUS, counts, 2)
    status, value = ina228_lib.read(board, "bus_voltage_V")
    assert status == ina228_lib.OK
    assert value == pytest.approx(volts, rel=1e-6)


def test_ALX1553_P223_a_bus_error_is_reported_and_not_turned_into_a_reading(ina228_lib, board):
    """When the I2C master fails, every getter returns the failure instead of a stale number.

    A sensor that answers with its previous value when the bus is down is worse than one that says
    nothing, because the caller has no way to tell. The fake is told to fail, and each reading is
    asked for the status rather than the value.
    """
    ina228_lib.set_register(REG_CURRENT, 100, 2)
    assert ina228_lib.read(board, "current_A") == (ina228_lib.OK, pytest.approx(1.25))

    ina228_lib.c.AlxI2cFake_SetForcedStatus(ina228_lib.ERR)
    for quantity in ("current_A", "bus_voltage_V", "shunt_voltage_V", "power_W"):
        status, _ = ina228_lib.read(board, quantity)
        assert status == ina228_lib.ERR, f"{quantity} did not report the bus failure"
    ina228_lib.c.AlxI2cFake_SetForcedStatus(ina228_lib.OK)


# =====================================================================
# P224-P226 - the wire, the limit, and the number ALX-1480 needs
# =====================================================================


def test_ALX1553_P224_the_driver_reverses_the_byte_order_of_every_register(ina228_lib, board):
    """The part sends most significant byte first; the struct behind it is little-endian.

    The driver reverses the bytes as it copies them in, and that single loop is the only thing
    standing between a 24 V reading and a 0.6 V one. Checked by putting a value on the bus whose two
    bytes differ and asking for the number it means.
    """
    ina228_lib.set_register(REG_VBUS, 0x1E00, 2)            # 7680 counts, MSB 0x1E, LSB 0x00
    status, value = ina228_lib.read(board, "bus_voltage_V")
    assert status == ina228_lib.OK
    assert value == pytest.approx(24.0), "0x1E00 read as 0x001E would be 0.09 V"

    ina228_lib.set_register(REG_VBUS, 0x001E, 2)
    _, swapped = ina228_lib.read(board, "bus_voltage_V")
    assert swapped == pytest.approx(0.09375), "and the other way round, to show the two differ"


def test_ALX1553_P225_the_die_temperature_cannot_be_tested_on_this_host(ina228_lib, board,
                                                                       record_property):
    """CHARACTERIZATION: the DIETEMP overlay is not ABI-portable, so this suite skips its arithmetic.

    The INA238 temperature register puts its 12 bits in the TOP of a 16-bit word, and the library
    describes that with two bit-fields of DIFFERENT declared types - ``int8_t unused_0_3 : 4`` then
    ``int16_t DIETEMP_mDegC : 12``. Under the GNU rules the target compiles with, those pack into one
    16-bit unit and the union is 2 bytes. Under the Microsoft rules clang follows on this host, a new
    declared type starts a new allocation unit, so the union is 4 BYTES and the temperature field
    lands outside the two bytes the register occupies.

    Measured here: writing 3200 to the union's raw word reads back 12 from the field, where the
    target would read 200 - which is 25.0 degC at 125 m degC per count.

    So the numbers this host produces for the temperature path describe the host's ABI, not the
    firmware, and no assertion is made about them. Two things are worth taking away. Any other
    register overlay that mixes bit-field types has the same problem, and the driver derives each
    register's I2C LENGTH from ``sizeof`` of that overlay - so on a toolchain that packs differently
    it would read the wrong number of bytes, silently. For ALX_INA238 the temperature register is the
    only one affected; the INA228 variant has three more.

    This also matters for the bench: `test_power.py` P93 records that this board's die temperature
    reads 0.0 almost always, and this test says the host cannot be used to investigate that.
    """
    ina228_lib.set_register(REG_DIETEMP, 3200, 2)
    status, value = ina228_lib.read(board, "temp_degC")
    record_property("host_dietemp_reading_degc", value)
    assert status == ina228_lib.OK, "the read still succeeds; it is the layout that differs"


def test_ALX1553_P226_what_the_benchs_own_numbers_are_in_counts(ina228_lib, board, record_property):
    """METRIC: the bench measurements of 2026-09-10, expressed in the units the part actually has.

    From `test_power.py` P92, with the supply delivering 0.041 A into the board:

        the driver's own reading            0.025 A    =  2 counts of 12.5 mA
        the true current                  0.041 A    =  3.28 counts
        the product's compensated value     0.1875 A   =  the reading multiplied by 7.5

    The last line is not this driver: the product multiplies the raw reading by 7.5 below 1.5 A
    (its own application code, marked `ToDo fix` there), which is where the seven and a half
    comes from.
    The driver's own reading is one count below where a part with no error at all would land -
    3.28 counts rounds to 3, and it gave 2 - and one count across a 100 uOhm shunt is 1.25 uV,
    far inside the part's specification. That reading is quantisation, not error.

    So the raw path is quantisation-limited at the bench's idle draw, and the compensated path is a
    curve. Neither is a mistake in the arithmetic tested above - which is the point of recording it
    here, next to the factors, rather than arguing about it on a ticket.
    """
    lsb = ina228_lib.current_lsb(board)
    for name, amps in (("raw", 0.025), ("true", 0.041), ("compensated", 0.1875)):
        record_property(f"bench_{name}_in_counts", f"{amps / lsb:.2f}")

    ina228_lib.set_register(REG_CURRENT, 2, 2)
    _, raw = ina228_lib.read(board, "current_A")
    assert raw == pytest.approx(0.025), "two counts is what the board reported"
    assert 0.041 / lsb == pytest.approx(3.28, abs=0.01), "the truth is three and a quarter counts"


# =====================================================================
# P227 - the defect the sweep found
# =====================================================================


def test_ALX1553_P227_the_calibration_register_is_4096_for_every_shunt(ina228_lib):
    """The mathematics: SHUNT_CAL does not depend on the shunt at all, and is always 4096.

    819.2e6 x CURRENT_LSB x SHUNT x MULT, where CURRENT_LSB is (RANGE / SHUNT) / 2^15 and MULT is 4
    on the narrow range and 1 on the wide one. The shunt cancels, the range cancels against MULT, and
    what is left is 819.2e6 x 40.96e-3 / 32768 x 4 = 4096 - the part's own reset value.

    Two shunts that land on it exactly, to show the arithmetic is right before P227's sibling shows
    where it is not.
    """
    for adc_range, shunt in (("RANGE_40_96_MV", 0.0001), ("RANGE_163_84_MV", 0.01)):
        sensor = ina228_lib.sensor(getattr(ina228_lib, adc_range), shunt)
        assert ina228_lib.shunt_cal(sensor) == 4096, f"{adc_range} with {shunt} Ohm"


@pytest.mark.xfail(
    strict=True,
    reason="AlxIna228_ShuntFactor truncates its float result with a (uint16_t) cast instead of "
           "rounding it, so a calibration that is mathematically exactly 4096 is written as 4095 "
           "whenever the float lands below the integer - which it does for a whole set of shunts",
)
@pytest.mark.parametrize("shunt_ohm", [0.00025, 0.0005, 0.001, 0.002])
@pytest.mark.parametrize("adc_range", ["RANGE_40_96_MV", "RANGE_163_84_MV"])
def test_ALX1553_P227_the_calibration_register_rounds_rather_than_truncates(
    ina228_lib, adc_range, shunt_ohm
):
    """SEALED DEFECT - a calibration that is exactly 4096 is written as 4095 for some shunts.

    ``AlxIna228_ShuntFactor`` finishes with

        me->conversionShuntFactor = (uint16_t)shuntCalRegVal;

    a TRUNCATING cast of a 32-bit float. The value it truncates is mathematically an exact integer -
    P227's sibling shows the shunt and the range cancelling out of it - but a float32 cannot hold
    every intermediate exactly, and for these four shunt values it lands just below 4096. Truncation
    then throws away the rest of the count.

    Swept across eleven shunts from 50 uOhm to 100 mOhm, on both ADC ranges, the answer is 4096 for
    seven of them and 4095 for four: 250 uOhm, 500 uOhm, 1 mOhm and 2 mOhm. There is nothing about
    those four that a designer could predict, and nothing tells them apart at runtime.

    One count in 4096 is 0.024 % of the current reading's scale - small, and it is a scale error
    rather than an offset, so it grows with the current. **This board is not affected**: its shunt is
    100 uOhm, which lands on 4096. The fix is a rounding cast.

    Strict xfail: XPASSes the day the cast rounds.
    """
    sensor = ina228_lib.sensor(getattr(ina228_lib, adc_range), shunt_ohm)
    assert ina228_lib.shunt_cal(sensor) == 4096
