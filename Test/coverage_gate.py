"""Coverage gate: every named file must be 100% in lines/branches/regions/functions.
Usage: coverage_gate.py <llvm-cov-summary.json> <file1.c> [file2.c ...]"""
import json
import sys

args = sys.argv[1:]
metrics = ("lines", "branches", "regions", "functions")
if "--metrics" in args:            # gate only these metrics (e.g. functions) - used where code is unreachable by design
    i = args.index("--metrics")
    metrics = tuple(args[i + 1].split(","))
    del args[i:i + 2]
summary_path, gate_files = args[0], set(args[1:])
data = json.load(open(summary_path))
bad = []
seen = set()
for f in data["data"][0]["files"]:
    name = f["filename"].replace("\\", "/").rsplit("/", 1)[-1]
    if name in gate_files:
        seen.add(name)
        for k in metrics:
            pct = f["summary"][k]["percent"]
            if pct < 100.0:
                bad.append((name, k, round(pct, 2)))
missing = gate_files - seen
if missing:
    bad.append(("MISSING FROM REPORT", ",".join(sorted(missing)), 0))
print("COVERAGE GATE:", "FAIL " + str(bad) if bad else f"PASS (100% on {list(metrics)} for {sorted(gate_files)})")
sys.exit(1 if bad else 0)
