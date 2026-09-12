"""ALX-1553 - the CLI answers nothing at all when its assertions are compiled out.

alxCli.c performs TWENTY-ONE of its serial writes inside ALX_CLI_ASSERT:

    ALX_CLI_ASSERT(AlxSerialPort_WriteStr(me->alxSerialPort, me->buff) == Alx_Ok);

A module compiled with no ALX_CLI_ASSERT_*_ENABLE expands that macro to `do {} while(false)` taking
no parameters, so the preprocessor deletes the argument - and the write with it. Not the check: the
WRITE. Every reply the CLI makes goes through one of those lines, so the whole interface falls
silent while every function still returns normally.

Measured on `help`, the same command test_alxCli.py drives:

    default        ASSERT_RST      751 bytes
    debug          ASSERT_BKPT     751 bytes
    assert_trace   ASSERT_TRACE    751 bytes
    off            (no form)         0 bytes

This is the same defect as P561 in AlxParamItem_SetValToDef, and the same one 3g describes from the
bench, but it is the widest instance in the library: the CLI is how a product is operated, and a
build that left ALX_CLI assertions out would answer nothing to anything, with no error anywhere to
say why. A survey of the library found 49 sites in five files with this shape - alxCli.c 21,
alxIoPin_McuZephyr.c 14 (gpio_pin_set, gpio_pin_configure - the GPIO writes themselves),
alxParamItem.c 7 (six of them the sprintf that renders a parameter's value), alxClk_McuLpc55S6x.c 5
(CLOCK_SetPLL0Freq - the clock setup) and alxNet.c 2 (the SPI transfers).

The fix is the same everywhere and is library source: perform the call on its own line, keep its
status, and assert on the status.

This group also exists because of what it cost to add: the CLI group already had its sources, its
closure and its .def, and only a line in VARIANT_GROUPS was missing. The four variants were built
for exactly this question and had never been pointed at the module where the answer matters most.

Proofs (ALX-1553): P566-P567.
"""

import pytest

from conftest import VARIANTS

pytestmark = pytest.mark.variants

HELP_MARK = b'"help":"Help Command'
ID_MARK = b'"id"'


def _ask(cli, data: bytes, handles: int = 3) -> bytes:
    cli.inject(data)
    for _ in range(handles):
        cli.handle()
    return cli.tx()


def test_ALX1553_P566_a_build_with_assertions_answers_a_command(
        cli_variant_lib, make_cli_variant):
    """The three configurations that compile an assert form all answer `help` in full."""
    name, _lib = cli_variant_lib
    form, _trace, _level = VARIANTS[name]
    if form is None:
        pytest.skip("the variant with no assert form is P567, the sealed half")

    response = _ask(make_cli_variant(), b"help\r\n")

    assert response, f"variant {name!r} answered nothing"
    assert HELP_MARK in response
    assert ID_MARK in response, "the reply is truncated, not merely present"


def test_ALX1553_P566_the_three_assert_forms_answer_identically(
        cli_variant_lib, make_cli_variant):
    """The assert FORM must not change what the CLI says - only what happens when one fails.

    RST, BKPT and TRACE differ in their handler, and all three keep the expression, so the reply
    has to be byte-for-byte the same. If it ever is not, the macro is doing more than it claims.
    """
    name, _lib = cli_variant_lib
    form, _trace, _level = VARIANTS[name]
    if form is None:
        pytest.skip("`off` keeps no expression at all - that is P567")

    response = _ask(make_cli_variant(), b"help\r\n")

    assert len(response) == 751, (
        f"variant {name!r} answered {len(response)} bytes where the other assert forms answer 751"
    )


@pytest.mark.xfail(strict=True, reason=(
    "DEFECT: alxCli.c performs 21 of its serial writes INSIDE ALX_CLI_ASSERT - for example "
    "ALX_CLI_ASSERT(AlxSerialPort_WriteStr(me->alxSerialPort, me->buff) == Alx_Ok) at alxCli.c:129. "
    "A module compiled with no ALX_CLI_ASSERT_*_ENABLE expands that macro to do {} while(false) "
    "taking no parameters, so the preprocessor deletes the argument and the WRITE with it. Every "
    "reply the CLI makes goes through one of those lines, so a build without this module's "
    "assertions answers NOTHING to any command while every function still returns normally and no "
    "error is raised anywhere. Measured on `help`: 751 bytes with RST, BKPT or TRACE, 0 bytes with "
    "no form. The CLI is how a product is operated, which makes this the widest instance of a shape "
    "the library repeats in 49 places across five files - alxIoPin_McuZephyr.c puts the gpio_pin_set "
    "calls themselves inside assertions, alxParamItem.c the sprintf that renders a value, "
    "alxClk_McuLpc55S6x.c the PLL setup. The fix is the same in all of them and is library source: "
    "make the call on its own line, keep the status, assert on the status"))
def test_ALX1553_P567_a_build_without_assertions_also_answers_a_command(
        cli_variant_lib, make_cli_variant):
    """The `off` variant, where the writes have been deleted along with the assertions.

    Only the configuration with no assert form is asked; the other three are P566 and skip here, so
    a fix makes this XPASS and fails the suite exactly once, in the one place that measures it.
    """
    name, _lib = cli_variant_lib
    form, _trace, _level = VARIANTS[name]
    if form is not None:
        pytest.skip("a variant that compiles assertions is P566")

    response = _ask(make_cli_variant(), b"help\r\n")

    assert response, "the CLI answered nothing at all"
    assert HELP_MARK in response
