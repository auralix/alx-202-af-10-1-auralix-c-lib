"""ALX-1514 - alxCli PC unit tests (L5, Tier 2 over alxSerialPortFake).

Contract under test (the sold feature - CLI accepts CR, LF, or CRLF):

  - a command line terminates on the FIRST of CR or LF (PuTTY sends bare CR);
    a CRLF pair yields the command on CR and a silently-ignored empty line on LF
  - an empty line (any terminator) is SILENT - no response, nothing consumed beyond it
  - an unknown command answers the "Command invalid" JSON error
  - an invalid command does NOT flush the RX FIFO - pipelined commands survive
  - an undeliverable line (ErrTooLong: line longer than the CLI buffer headroom,
    or FIFO full without terminator) answers "Command invalid"; the CLI stays
    functional afterwards
  - output side unchanged: responses always terminated with CRLF pairs
  - reset command responds success, then requests NVIC_SystemReset (shimmed)

Test group P14 = alxCli terminator/robustness proofs. The same terminator
matrix runs on the real device as P13 (HIL, device repo Test/).
"""

import pytest

pytestmark = pytest.mark.unit

HELP_MARK = b'"help":"Help Command'
INVALID_MARK = b"Command invalid"


def cmd(cli, data: bytes, handles: int = 3) -> bytes:
    """Inject bytes, run the CLI poll loop a few times, drain the response."""
    cli.inject(data)
    for _ in range(handles):
        cli.handle()
    return cli.tx()


# =====================================================================
# P14 - terminator acceptance (the sold feature)
# =====================================================================

def test_ALX1514_P14_crlf_terminates_command(make_cli):
    cli = make_cli()
    resp = cmd(cli, b"help\r\n")
    assert HELP_MARK in resp
    assert INVALID_MARK not in resp          # the trailing LF is a SILENT empty line


def test_ALX1514_P14_cr_terminates_command(make_cli):
    """PuTTY's Enter = bare CR - the reason this task exists."""
    cli = make_cli()
    resp = cmd(cli, b"help\r")
    assert HELP_MARK in resp


def test_ALX1514_P14_lf_terminates_command(make_cli):
    cli = make_cli()
    resp = cmd(cli, b"help\n")
    assert HELP_MARK in resp


def test_ALX1514_P14_crlf_pair_yields_exactly_one_response(make_cli):
    cli = make_cli()
    resp = cmd(cli, b"help\r\n", handles=4)
    assert resp.count(HELP_MARK) == 1
    assert INVALID_MARK not in resp


def test_ALX1514_P14_split_delivery_one_command(make_cli):
    """Bytes trickle in across poll cycles (real UART reality)."""
    cli = make_cli()
    cli.inject(b"he"); cli.handle()
    cli.inject(b"lp"); cli.handle()
    resp = cmd(cli, b"\r")
    assert resp.count(HELP_MARK) == 1


# =====================================================================
# P14 - empty line is silent
# =====================================================================

@pytest.mark.parametrize("term", [b"\r", b"\n", b"\r\n"])
def test_ALX1514_P14_empty_line_is_silent(make_cli, term):
    cli = make_cli()
    resp = cmd(cli, term)
    assert resp == b""


def test_ALX1514_P14_empty_lines_then_command_works(make_cli):
    cli = make_cli()
    assert cmd(cli, b"\r\n\r\n\r") == b""
    assert HELP_MARK in cmd(cli, b"help\r")


# =====================================================================
# P14 - invalid command, pipeline survival (RX flush removed)
# =====================================================================

def test_ALX1514_P14_unknown_command_answers_invalid(make_cli):
    cli = make_cli()
    resp = cmd(cli, b"bogus\r")
    assert INVALID_MARK in resp


def test_ALX1514_P14_invalid_command_does_not_flush_pipeline(make_cli):
    """Old CLI flushed the whole RX FIFO on an invalid command, killing any
    pipelined valid command behind it. The new primitive consumes exactly one
    line, so the flush is gone - both commands must answer."""
    cli = make_cli()
    cli.inject(b"bogus\rhelp\r")
    for _ in range(4):
        cli.handle()
    resp = cli.tx()
    assert INVALID_MARK in resp
    assert HELP_MARK in resp


# =====================================================================
# P14 - buffer headroom (D1) and undeliverable lines (ErrTooLong)
# =====================================================================

def test_ALX1514_P14_max_length_line_no_overflow_then_too_long_invalid(make_cli, cli_lib):
    """D1 proof: a command of exactly the maximum deliverable length must be
    handled (as unknown -> invalid) WITHOUT corruption, and one char more is
    undeliverable (ErrTooLong) -> also 'Command invalid'; the CLI keeps
    working after both. ASan/UBSan variants watch the memory side."""
    buff_len = cli_lib.buff_len()
    max_content = buff_len - 3          # read len = buffLen-1, content incl. terminator = buffLen-2

    cli = make_cli()
    resp = cmd(cli, b"A" * max_content + b"\r", handles=4)
    assert INVALID_MARK in resp
    assert HELP_MARK in cmd(cli, b"help\r")

    cli2 = make_cli()
    resp = cmd(cli2, b"A" * (max_content + 1) + b"\r", handles=4)
    assert INVALID_MARK in resp
    assert HELP_MARK in cmd(cli2, b"help\r")


# =====================================================================
# P14 - reset command (NVIC shim)
# =====================================================================

def test_ALX1514_P14_reset_command_responds_then_resets(make_cli, cli_lib):
    cli = make_cli()
    cli_lib.clear_reset()
    resp = cmd(cli, b"reset\r")
    assert b'"status":"success"' in resp
    assert cli_lib.was_reset()


# =====================================================================
# P14 - output side unchanged (always CRLF)
# =====================================================================

def test_ALX1514_P14_responses_use_crlf_line_endings(make_cli):
    cli = make_cli()
    resp = cmd(cli, b"help\r")
    assert b"\r\n" in resp
    assert b"\n" not in resp.replace(b"\r\n", b"")   # no bare LF anywhere
