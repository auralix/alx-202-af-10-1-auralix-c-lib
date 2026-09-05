"""Auralix C Library - QUALIFY step: mutation run (tests the tests).

For each generated mutant of a gated source: plant it over the original, rebuild
the dev DLL, run the suite.
  suite red or hang  -> KILLED          (the tests noticed the planted bug)
  suite green        -> SURVIVED        (hole in the tests - or an equivalent mutant;
                                         judge by the diff in build/mutation/survivors/)
  -Werror build fail -> KILLED_COMPILE  (syntax-valid but caught by the strict COMPILE
                                         gate before any test ran; scored separately)

Two generation-time filters keep the pool honest (both cached until src changes):
  1. clang -fsyntax-only        drops syntax-invalid mutants (regex rules make many)
  2. TCE (trivial compile equivalence): mutant compiled to an object under a fixed
     name; byte-identical to the original's object -> provably equivalent, dropped
     (comment edits, disabled-assert arguments, dead code).
Suite kill rate = killed / (killed + survived).

Report-only: exits 0 unless the runner itself fails. Kill rate 100% is NOT the
target (equivalent mutants exist) - the deliverable is the survivor diffs.
Mutants are cached in build/mutation/mutants/ until the source changes. The
original file is always restored (finally), then rebuilt and the full suite
re-run as proof of restoration.
"""

import argparse
import difflib
import json
import random
import subprocess
import sys
from pathlib import Path

import conftest  # single source of truth for compiler path and build recipe

TEST_DIR = Path(__file__).parent
CLIB_DIR = TEST_DIR.parent
MUT_ROOT = TEST_DIR / "build" / "mutation"


def _run(cmd, timeout):
    return subprocess.run(cmd, cwd=TEST_DIR, capture_output=True, text=True, timeout=timeout)


_CLANG_FRONT = ["-w", "-std=gnu99", "-D_CRT_SECURE_NO_WARNINGS"]


def _clang(path: Path, extra, timeout=60):
    return _run([conftest.CLANG, *extra, *_CLANG_FRONT,
                 f"-I{TEST_DIR}", f"-I{CLIB_DIR}", f"-I{CLIB_DIR / 'Mcu'}",
                 str(path)], timeout=timeout)


def _generate(src: Path, mdir: Path):
    """Generate + filter mutants (syntax, then TCE); cached until src changes.

    Returns (viable_mutants, counts_dict)."""
    stamp = mdir / ".tce.json"
    if stamp.exists() and stamp.stat().st_mtime > src.stat().st_mtime:
        return sorted(mdir.glob(src.stem + ".mutant.*.c")), json.loads(stamp.read_text())
    mdir.mkdir(parents=True, exist_ok=True)
    for f in mdir.iterdir():
        f.unlink()
    print(f"generating mutants for {src.name} ...")
    r = _run([sys.executable, "-m", "universalmutator.genmutants", str(src), "c",
              "--noCheck", "--mutantDir", str(mdir)], timeout=1200)
    if r.returncode != 0:
        raise RuntimeError(f"mutant generation failed:\n{r.stdout}\n{r.stderr}")
    generated = sorted(mdir.glob(src.stem + ".mutant.*.c"))

    print(f"syntax pre-filter on {len(generated)} mutants (one-time, cached) ...")
    viable = []
    for i, m in enumerate(generated, 1):
        if _clang(m, ["-fsyntax-only"]).returncode == 0:
            viable.append(m)
        else:
            m.unlink()
        if i % 200 == 0:
            print(f"  {i}/{len(generated)} checked, {len(viable)} viable")

    # TCE: same object code as the original (under a FIXED file name, so the
    # embedded source name can't fake a difference) = provably equivalent.
    def _obj_normalized() -> bytes:
        # COFF header TimeDateStamp (bytes 4..8) is the object's ONE
        # nondeterministic field (measured: 1 differing byte at offset 4
        # between compiles of identical input) - zero it before comparing
        b = tce_obj.read_bytes()
        return b[:4] + b"\x00\x00\x00\x00" + b[8:]

    tce_src, tce_obj = mdir / "_tce.c", mdir / "_tce.o"
    obj = ["-c", "-O1", "-o", str(tce_obj)]
    tce_src.write_bytes(src.read_bytes())
    base = _obj_normalized() if _clang(tce_src, obj).returncode == 0 else None
    if base is not None:  # determinism sanity: same input twice -> same bytes
        if _clang(tce_src, obj).returncode != 0 or _obj_normalized() != base:
            base = None
    equivalent = 0
    if base is None:
        print("  TCE skipped: object compare not deterministic on this setup")
    else:
        print(f"TCE filter on {len(viable)} mutants (one-time, cached) ...")
        kept = []
        for i, m in enumerate(viable, 1):
            tce_src.write_bytes(m.read_bytes())
            if _clang(tce_src, obj).returncode == 0 and _obj_normalized() == base:
                m.unlink()
                equivalent += 1
            else:
                kept.append(m)
            if i % 100 == 0:
                print(f"  {i}/{len(viable)} compared, {equivalent} equivalent dropped")
        viable = kept
    for f in (tce_src, tce_obj):
        if f.exists():
            f.unlink()

    counts = {"generated": len(generated), "syntax_viable": len(viable) + equivalent,
              "tce_equivalent": equivalent, "pool": len(viable)}
    stamp.write_text(json.dumps(counts))
    return viable, counts


BUILDERS = {                       # source under mutation -> the conftest builder of its test group DLL
    "alxFifo.c": "_build_fifo_dll", "alxBound.c": "_build_fifo_dll",
    "alxCli.c": "_build_cli_dll",
    "alxMemSafe.c": "_build_memsafe_dll", "alxCrc.c": "_build_memsafe_dll",
    "alxParamGroup.c": "_build_memsafe_dll", "alxParamStore.c": "_build_memsafe_dll",
}


def _build(src_name: str = "alxFifo.c") -> bool:
    builder = BUILDERS.get(src_name, "_build_fifo_dll")
    r = _run([sys.executable, "-c", f"import conftest; conftest.{builder}()"], timeout=300)
    return r.returncode == 0


def _suite() -> str:
    """'green' | 'red' | 'hang' - fast fail, fixed order, no evidence reports."""
    try:
        r = _run([sys.executable, "-m", "pytest", "-q", "-x", "-p", "no:randomly",
                  "-o", "addopts="], timeout=180)
    except subprocess.TimeoutExpired:
        return "hang"
    return "green" if r.returncode == 0 else "red"


def qualify(src_name: str, sample: int, seed: int, report_lines: list) -> int:
    src = CLIB_DIR / src_name
    mdir = MUT_ROOT / "mutants" / src.stem   # PER-SOURCE pool: a shared dir made the 2nd
                                             # source hit the 1st source's cache stamp (0 run)
    surv_dir = MUT_ROOT / "survivors"
    surv_dir.mkdir(parents=True, exist_ok=True)
    for f in surv_dir.glob(src.stem + ".mutant.*.diff"):   # stale diffs mislead
        f.unlink()

    mutants, counts = _generate(src, mdir)
    selected = mutants
    if sample and sample < len(mutants):
        rnd = random.Random(seed)
        selected = sorted(rnd.sample(mutants, sample), key=lambda p: p.name)
    pool_desc = (f"{counts['generated']} generated, {counts['syntax_viable']} syntax-viable, "
                 f"{counts['tce_equivalent']} TCE-equivalent dropped -> pool {counts['pool']}")
    print(f"{src_name}: {pool_desc}, running {len(selected)} "
          f"({'sampled, seed ' + str(seed) if len(selected) < len(mutants) else 'all'})")

    original = src.read_bytes()
    orig_text = original.decode("ascii", "replace").splitlines()
    killed = survived = killed_compile = 0
    survivors = []
    try:
        for i, m in enumerate(selected, 1):
            src.write_bytes(m.read_bytes())
            if not _build(src_name):
                killed_compile += 1          # syntax-valid, but the -Werror gate got it
                verdict = "KILLED_COMPILE (-Werror gate)"
            else:
                outcome = _suite()
                if outcome == "green":
                    survived += 1
                    verdict = "SURVIVED"
                    mut_text = m.read_bytes().decode("ascii", "replace").splitlines()
                    diff = "\n".join(difflib.unified_diff(
                        orig_text, mut_text, fromfile=src_name, tofile=m.name, lineterm="", n=2))
                    (surv_dir / (m.stem + ".diff")).write_text(diff + "\n", encoding="ascii", errors="replace")
                    survivors.append(m.name)
                else:
                    killed += 1
                    verdict = f"KILLED ({outcome})"
            print(f"  [{i}/{len(selected)}] {m.name}: {verdict}")
    finally:
        src.write_bytes(original)

    scored = killed + survived
    rate = (100.0 * killed / scored) if scored else 0.0
    report_lines += [
        f"{src_name}: {pool_desc}, {len(selected)} run "
        f"({'sample' if len(selected) < len(mutants) else 'all'}): "
        f"{killed} killed by suite, {killed_compile} killed by -Werror gate, {survived} survived "
        f"-> suite kill rate {rate:.1f}% of {scored} suite-scored",
        *[f"  SURVIVOR: {n}  (diff: build/mutation/survivors/{Path(n).stem}.diff)" for n in survivors],
    ]
    return survived


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("sources", nargs="*", help="c-lib sources to mutate (default: alxFifo.c)")
    ap.add_argument("--sample", type=int, default=150, help="mutants per source, 0 = all")
    ap.add_argument("--seed", type=int, default=1514, help="sampling seed (deterministic)")
    args = ap.parse_args()

    report = []
    for name in (args.sources or ["alxFifo.c"]):
        qualify(name, args.sample, args.seed, report)

    print("\nrestoration proof: rebuild + full suite on the original source ...")
    if not all(_build(n) for n in (args.sources or ["alxFifo.c"])):
        print("FATAL: rebuild after restore failed"); return 2
    r = _run([sys.executable, "-m", "pytest", "-q"], timeout=300)
    if r.returncode != 0:
        print("FATAL: suite red after restore"); return 2
    print("restored: dev DLL rebuilt, full suite green")

    print("\n" + "=" * 70)
    for line in report:
        print(line)
    (MUT_ROOT / "report.txt").write_text("\n".join(report) + "\n", encoding="ascii", errors="replace")
    print(f"report: {MUT_ROOT / 'report.txt'}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
