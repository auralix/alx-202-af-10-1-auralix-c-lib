"""Auralix C Library - style gate (ANALYZE Stage 0).

Two mechanical rules on the gated sources:

  1. NO TERNARY OPERATOR. Outside comments/strings/char literals, '?' has exactly
     one meaning in C: the conditional operator. Rule: write if/else instead.

  2. DOXYGEN TAG ALIGNMENT. In every /** ... */ block (tab stop = 4, per
     .editorconfig): tag lines use TABS ONLY between fields, the name/value
     column is identical across the block, and the description column is
     identical across the block (@brief's text counts as a description).

Usage:  python style_gate.py <file> [<file> ...]     Exit 1 on any finding.
"""

import re
import sys

TABSTOP = 4
DESC_ONLY_TAGS = {"brief", "note", "return", "details"}
NAMED_TAGS = {"param", "retval"}
_TAG_RE = re.compile(r"^(\s*\*\s*)(@\w+(?:\[[^\]]*\])?)(.*)$")


def find_ternaries(text):
    """Line numbers of '?' in code (comments/strings/chars stripped by state machine)."""
    CODE, LINE_C, BLOCK_C, STR, CHR = range(5)
    state, escape, line, hits = CODE, False, 1, []
    i, n = 0, len(text)
    while i < n:
        c = text[i]
        nxt = text[i + 1] if i + 1 < n else ""
        if c == "\n":
            line += 1
            if state == LINE_C:
                state = CODE
        elif state == CODE:
            if c == "/" and nxt == "/":
                state, i = LINE_C, i + 1
            elif c == "/" and nxt == "*":
                state, i = BLOCK_C, i + 1
            elif c == '"':
                state = STR
            elif c == "'":
                state = CHR
            elif c == "?":
                hits.append(line)
        elif state == BLOCK_C:
            if c == "*" and nxt == "/":
                state, i = CODE, i + 1
        elif state in (STR, CHR):
            if escape:
                escape = False
            elif c == "\\":
                escape = True
            elif (state == STR and c == '"') or (state == CHR and c == "'"):
                state = CODE
        i += 1
    return hits


def check_doc_block(path, lines, start, findings):
    """lines[start] is the line after '/**'; returns index of the '*/' line.

    Collects per-block name columns and description columns; each set must be
    single-valued. Field separators must be tabs only.
    """
    name_cols, desc_cols = {}, {}
    i = start
    while i < len(lines) and "*/" not in lines[i]:
        raw = lines[i]
        m = _TAG_RE.match(raw)
        if m:
            _prefix, tag_tok, rest = m.groups()
            tag = tag_tok[1:].split("[")[0]
            if tag in DESC_ONLY_TAGS or tag in NAMED_TAGS:
                sep = rest[: len(rest) - len(rest.lstrip())]
                if " " in sep:
                    findings.append(f"{path}:{i + 1}: spaces in field separator after {tag_tok} (tabs only)")
                exp = raw.expandtabs(TABSTOP)
                end_tag = exp.index(tag_tok) + len(tag_tok)
                tail = exp[end_tag:]
                field = tail.lstrip()
                if field:
                    col1 = end_tag + (len(tail) - len(field))
                    if tag in DESC_ONLY_TAGS:
                        desc_cols.setdefault(col1, []).append(i + 1)
                    else:
                        name_cols.setdefault(col1, []).append(i + 1)
                        name = field.split()[0]
                        pos = col1 + len(name)
                        tail2 = exp[pos:]
                        desc = tail2.lstrip()
                        raw_after_name = raw[raw.index(name, raw.index(tag_tok)) + len(name):]
                        sep2 = raw_after_name[: len(raw_after_name) - len(raw_after_name.lstrip())]
                        if desc and " " in sep2:
                            findings.append(f"{path}:{i + 1}: spaces in field separator after '{name}' (tabs only)")
                        if desc:
                            desc_cols.setdefault(pos + (len(tail2) - len(desc)), []).append(i + 1)
        i += 1
    for label, cols in (("name/value", name_cols), ("description", desc_cols)):
        if len(cols) > 1:
            detail = "; ".join(f"col {c} -> line(s) {v}" for c, v in sorted(cols.items()))
            findings.append(f"{path}:{start}: doc block {label} columns not aligned: {detail}")
    return i


def check_file(path):
    findings = []
    text = open(path, encoding="ascii", errors="strict").read()
    for line in find_ternaries(text):
        findings.append(f"{path}:{line}: ternary operator (write if/else)")
    lines = text.splitlines()
    i = 0
    while i < len(lines):
        if lines[i].strip().startswith("/**"):
            i = check_doc_block(path, lines, i + 1, findings)
        i += 1
    return findings


def main(argv):
    all_findings = []
    for path in argv:
        all_findings += check_file(path)
    for f in all_findings:
        print(f)
    if all_findings:
        print(f"STYLE GATE: FAIL ({len(all_findings)} finding(s))")
        return 1
    print(f"STYLE GATE: PASS ({len(argv)} files)")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
