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

import json

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
    cli.inject(b"he")
    cli.handle()
    cli.inject(b"lp")
    cli.handle()
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
    """Boundary proof: a command of exactly the maximum deliverable length must be
    handled (as unknown -> invalid) WITHOUT corruption, and one char more is
    undeliverable (ErrTooLong) -> also 'Command invalid'; the CLI keeps
    working after both. ASan/UBSan variants watch the memory side."""
    buff_len = cli_lib.buff_len()
    max_content = buff_len - 2          # read len = buffLen: content + terminator <= buffLen-1

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


# =====================================================================
# P15 - post-review CLI contract (design review 03.09):
#   - the terminator is stripped once before dispatch (commands match "help", not "help\r\n")
#   - set-param parses IN PLACE: key/val point into the line buffer, no stack copies,
#     so a value is bounded only by the CLI buffer and may contain spaces
#   - get-param formats the value straight into the CLI buffer (no fixed val[] array)
#   - the helper's param table has all three value kinds: bool, str (600 B buffer), uint8 (0..100)
# =====================================================================

ARGS_INVALID_MARK = b"Arguments invalid"
SUCCESS_MARK = b'"status":"success"'


def as_json(resp: bytes):
    """Every CLI response (pretty or compact) must be one JSON document once CRLFs are removed."""
    return json.loads(resp.replace(b"\r\n", b"").decode("ascii"))


def set_param(cli, key: bytes, val: bytes, term: bytes = b"\r") -> bytes:
    return cmd(cli, b"set-param --key " + key + b" --val " + val + term, handles=4)


def test_ALX1514_P15_set_param_str_value_with_spaces_round_trips(make_cli):
    """The in-place parser takes the REST OF THE LINE as the value, so string
    values may contain spaces (sscanf %s stopped at the first one)."""
    cli = make_cli()
    val = b"hello wide world 123"
    assert SUCCESS_MARK in set_param(cli, b"STR_TEST", val)
    resp = cmd(cli, b"get-param\r", handles=4)
    assert b'"STR_TEST":"' + val + b'"' in resp
    assert as_json(resp)["data"]["STR_TEST"] == val.decode()


def test_ALX1514_P15_set_param_long_str_value_round_trips(make_cli, cli_lib):
    """A value far longer than any stack array the old parser had (500 chars)
    goes through unchanged - proves no fixed-size copy is left in the path."""
    cli = make_cli()
    val = bytes(b"ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"[i % 36] for i in range(500))
    assert len(val) < cli_lib.str_val_buff_len()
    assert SUCCESS_MARK in set_param(cli, b"STR_TEST", val)
    resp = cmd(cli, b"get-param\r", handles=4)
    assert as_json(resp)["data"]["STR_TEST"] == val.decode()


def test_ALX1514_P15_set_param_str_value_too_long_for_param_rejected(make_cli, cli_lib):
    """The value limit is the PARAM's buffer (ParamItem), not the CLI: one char
    over -> 'Arguments invalid', old value kept, CLI alive."""
    cli = make_cli()
    assert SUCCESS_MARK in set_param(cli, b"STR_TEST", b"keep")
    too_long = b"X" * cli_lib.str_val_buff_len()          # buffer len = value + NUL -> one too many
    resp = set_param(cli, b"STR_TEST", too_long)
    assert ARGS_INVALID_MARK in resp
    assert as_json(cmd(cli, b"get-param\r", handles=4))["data"]["STR_TEST"] == "keep"


@pytest.mark.parametrize("line", [
    b"set-param --key STR_TEST",                 # no --val
    b"set-param --val 5 --key UINT8_TEST",       # swapped order
    b"set-param --key --val 5",                  # empty key
    b"set-param --key UINT8_TEST --val ",        # empty value
    b"set-param --key NO_SUCH_KEY --val 5",      # unknown key
    b"set-param --key PRETTY_JSON_EN --val maybe",  # not a bool
    b"set-param --key UINT8_TEST --val 200",     # out of range (valMax 100, Ignore)
])
def test_ALX1514_P15_set_param_bad_lines_answer_arguments_invalid(make_cli, line):
    cli = make_cli()
    assert ARGS_INVALID_MARK in cmd(cli, line + b"\r", handles=4)
    assert HELP_MARK in cmd(cli, b"help\r")


@pytest.mark.parametrize("line", [b"set-param", b"set-paramX --key UINT8_TEST --val 1",
                                  b"set-parameter --key UINT8_TEST --val 1"],
                         ids=["bare", "suffix", "longer-word"])
def test_ALX1514_P15_set_param_is_matched_as_a_whole_word(make_cli, line):
    """Dispatch is 'set-param ' (word + space, design review 03.09): a bare or misspelled word is
    not a command. Before: prefix match -> 'set-paramX --key K --val V' EXECUTED the set."""
    cli = make_cli()
    assert INVALID_MARK in cmd(cli, line + b"\r", handles=4)
    assert as_json(cmd(cli, b"get-param\r", handles=4))["data"]["UINT8_TEST"] == 7   # untouched
    assert HELP_MARK in cmd(cli, b"help\r")


def test_ALX1514_P15_set_param_uint8_round_trip(make_cli):
    cli = make_cli()
    assert as_json(cmd(cli, b"get-param\r", handles=4))["data"]["UINT8_TEST"] == 7   # valDef
    assert SUCCESS_MARK in set_param(cli, b"UINT8_TEST", b"42")
    assert as_json(cmd(cli, b"get-param\r", handles=4))["data"]["UINT8_TEST"] == 42


def test_ALX1514_P15_pretty_json_toggle_changes_response_format(make_cli):
    """PRETTY_JSON_EN is read at response time: the success response to the
    set itself is already compact, and so is the next get-param; back to pretty
    after. (help is NOT covered: its JSON is hard-coded pretty regardless of the
    flag - pre-existing behavior, reported upstream 03.09.)"""
    cli = make_cli()
    resp = set_param(cli, b"PRETTY_JSON_EN", b"false")
    assert resp == b'{"status":"success"}\r\n'
    compact = cmd(cli, b"get-param\r", handles=4)
    assert compact.startswith(b'{"status":"success","data":{')
    assert b"    " not in compact
    assert as_json(compact)["data"]["PRETTY_JSON_EN"] is False
    assert SUCCESS_MARK in set_param(cli, b"PRETTY_JSON_EN", b"true")
    assert cmd(cli, b"get-param\r", handles=4).startswith(b"{\r\n    ")


@pytest.mark.parametrize("line", [b"help", b"get", b"get-param", b"bogus",
                                  b"set-param --key UINT8_TEST --val 1"])
def test_ALX1514_P15_every_response_is_valid_json(make_cli, line):
    """The get-param body is now assembled piecewise into the CLI buffer
    (key, quotes, value, comma) - json.loads catches any slip in that assembly."""
    cli = make_cli()
    doc = as_json(cmd(cli, line + b"\r", handles=4))
    assert doc["status"] in ("success", "error")


def test_ALX1514_P15_get_param_lists_every_param_with_its_kind(make_cli):
    cli = make_cli()
    data = as_json(cmd(cli, b"get-param\r", handles=4))["data"]
    assert set(data) == {"PRETTY_JSON_EN", "STR_TEST", "UINT8_TEST"}
    assert data["PRETTY_JSON_EN"] is True          # bool unquoted
    assert data["STR_TEST"] == ""                  # str quoted (empty default)
    assert data["UINT8_TEST"] == 7                 # number unquoted


# =====================================================================
# Moved here with their proof tokens (ALX-1564) from a consumer's host suite, which drove them
# through its own parameter table: the behaviour is alxCli's, so the proof is this suite's. The
# consumer keeps the tests that need its own table, buffers and store.
# =====================================================================

@pytest.mark.parametrize("fragmentation", ["bytes", "words", "before_value"])
def test_ALX1564_P174_partial_commands_wait_for_terminator(make_cli, fragmentation):
    """A command arriving in pieces is not executed before its terminator.

    Every executed command answers, so no answer means nothing ran; the value the command sets
    appears only once the terminator is in.
    """
    cli = make_cli()
    line = b"set-param --key UINT8_TEST --val 54"
    if fragmentation == "bytes":
        fragments = [line[i:i + 1] for i in range(len(line))]
    elif fragmentation == "words":
        fragments = [b"set-param ", b"--key ", b"UINT8_TEST ", b"--val ", b"54"]
    else:
        fragments = [line[:-2], line[-2:]]
    for fragment in fragments:
        assert cmd(cli, fragment) == b"", "an answer before the terminator"
    assert SUCCESS_MARK in cmd(cli, b"\r", handles=4)
    assert as_json(cmd(cli, b"get-param\r", handles=4))["data"]["UINT8_TEST"] == 54


def test_ALX1564_P175_queued_set_and_get_commands_preserve_reply_order(make_cli):
    """Commands queued in one burst answer in the order they were sent, each with its own state."""
    cli = make_cli()
    values = [0, 100, 1, 50, 22, 99]
    burst = b"".join(b"set-param --key UINT8_TEST --val %d\rget-param\r" % v for v in values)
    cli.inject(burst)
    output = b""
    for _ in range(4 * len(burst)):
        cli.handle()
        output += cli.tx()
    decoder = json.JSONDecoder()
    remaining = output.replace(b"\r\n", b"").decode("ascii")
    frames = []
    while remaining:
        frame, end = decoder.raw_decode(remaining)
        frames.append(frame)
        remaining = remaining[end:]
    assert len(frames) == 2 * len(values)
    for index, value in enumerate(values):
        assert frames[2 * index]["status"] == "success"
        assert frames[2 * index + 1]["data"]["UINT8_TEST"] == value


@pytest.mark.parametrize("pretty", [b"true", b"false"])
@pytest.mark.parametrize("command", [b"get", b"get-param"])
@pytest.mark.parametrize("value", [b'a"b', b"a\tb"], ids=["quote", "tab"])
def test_ALX1564_P176_accepted_strings_produce_valid_json(make_cli, request, pretty, command, value):
    cli = make_cli()
    assert SUCCESS_MARK in set_param(cli, b"PRETTY_JSON_EN", pretty)
    assert SUCCESS_MARK in set_param(cli, b"STR_TEST", value)
    resp = cmd(cli, command + b"\r", handles=4)
    # Only the confirmed serializer defect is expected; a corrected serializer is a strict XPASS.
    request.node.add_marker(pytest.mark.xfail(
        strict=True, raises=json.JSONDecodeError,
        reason="ALX-1564: alxCli emits accepted string quotes and tabs without JSON escaping",
    ))
    assert as_json(resp)["data"]["STR_TEST"] == value.decode()


@pytest.mark.parametrize("pretty", [b"true", b"false"])
@pytest.mark.parametrize("command", [b"get", b"get-param"])
def test_ALX1564_P177_string_backslashes_round_trip_through_json(make_cli, request, pretty, command):
    cli = make_cli()
    value = rb"a\nb"
    assert SUCCESS_MARK in set_param(cli, b"PRETTY_JSON_EN", pretty)
    assert SUCCESS_MARK in set_param(cli, b"STR_TEST", value)
    response = as_json(cmd(cli, command + b"\r", handles=4))
    assert response["status"] == "success"
    request.node.add_marker(pytest.mark.xfail(
        strict=True, raises=AssertionError,
        reason="ALX-1564: alxCli emits literal backslashes as JSON escapes, changing the string",
    ))
    assert response["data"]["STR_TEST"] == value.decode()


@pytest.mark.parametrize("pretty", [b"true", b"false"])
@pytest.mark.parametrize("command", [b"get", b"get-param"])
@pytest.mark.parametrize("value", [b"PlainText", b"path/to.file-123_ABC"], ids=["plain", "punctuation"])
def test_ALX1564_P178_ordinary_strings_round_trip_through_json(make_cli, pretty, command, value):
    cli = make_cli()
    assert SUCCESS_MARK in set_param(cli, b"PRETTY_JSON_EN", pretty)
    assert SUCCESS_MARK in set_param(cli, b"STR_TEST", value)
    response = as_json(cmd(cli, command + b"\r", handles=4))
    assert response["status"] == "success"
    assert response["data"]["STR_TEST"] == value.decode()


@pytest.mark.parametrize("line", [
    pytest.param(b"set-param --key NO_SUCH_KEY --val 1", id="unknown_key"),
    pytest.param(b"set-param --key UINT8_TEST", id="missing_value"),
    pytest.param(b"set-param --key UINT8_TEST --val word", id="invalid_integer"),
    pytest.param(b"set-param --key PRETTY_JSON_EN --val word", id="invalid_bool"),
    pytest.param(b"set-param --val 1", id="missing_key"),
    pytest.param(b"set-param --key UINT8_TEST --val 200", id="out_of_range"),
    pytest.param(b"unknown-command", id="unknown_command"),
])
def test_ALX1564_P340_a_rejected_command_keeps_every_parameter_and_the_cli_recovers(make_cli, line):
    """A rejected command leaves every parameter as it was, and the next command is taken.

    The library-level half of what a consumer proves through its own store and table: the command
    line is this library's, so what a rejection leaves behind is this suite's to prove.
    """
    cli = make_cli()
    assert SUCCESS_MARK in set_param(cli, b"STR_TEST", b"edited")
    assert SUCCESS_MARK in set_param(cli, b"UINT8_TEST", b"42")
    before = as_json(cmd(cli, b"get-param\r", handles=4))["data"]
    assert as_json(cmd(cli, line + b"\r", handles=4))["status"] == "error"
    assert as_json(cmd(cli, b"get-param\r", handles=4))["data"] == before
    assert SUCCESS_MARK in set_param(cli, b"UINT8_TEST", b"43")
    assert as_json(cmd(cli, b"get-param\r", handles=4))["data"]["UINT8_TEST"] == 43
