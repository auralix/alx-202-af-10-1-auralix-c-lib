"""Auralix C Library - the C recipe of the MUTATE lane: three hooks for alx.verify.mutation.

    python Verify/mutation_hooks.py check <mutant.c>       exit 0 = compiles (clang -fsyntax-only), else STILLBORN
    python Verify/mutation_hooks.py fingerprint <file.c>   prints the sha256 of the object code compiled under a
                                                            fixed file name (COFF timestamp zeroed); equal
                                                            fingerprints = EQUIVALENT (trivial compile equivalence)
    python Verify/mutation_hooks.py rebuild                rebuilds every stale test DLL with conftest's -Werror
                                                            recipe; exit != 0 = KILLED_COMPILE

The MUTATE lane (Test/noxfile.py, `uv run nox -s mutate`) hands these to the driver (alx.verify.mutation) as
--check-cmd / --fingerprint-cmd / --rebuild-cmd. The driver generates the mutants with universalmutator, plants
them, runs the mirror test module, classifies, restores (with a crash-safe backup) and reports; only what is C
lives here.
"""

import hashlib
import subprocess
import sys
from pathlib import Path

TEST_DIR = Path(__file__).resolve().parents[1]
CLIB_DIR = TEST_DIR.parent
sys.path.insert(0, str(TEST_DIR))
import conftest  # noqa: E402  single source of truth for the compiler path and the build recipe

FRONT = ["-w", "-std=gnu99", "-D_CRT_SECURE_NO_WARNINGS",
         f"-I{TEST_DIR}", f"-I{CLIB_DIR}", f"-I{CLIB_DIR / 'Mcu'}"]
WORK = TEST_DIR / "build" / "mutate" / "_tce"     # fixed file names: the embedded source name must
                                                  # not fake a difference between two compiles


def _clang(*args, timeout=120):
    return subprocess.run([conftest.CLANG, *FRONT, *args], capture_output=True, text=True, timeout=timeout)


def check(path: str) -> int:
    return 0 if _clang("-fsyntax-only", path).returncode == 0 else 1


def fingerprint(path: str) -> int:
    WORK.mkdir(parents=True, exist_ok=True)
    src, obj = WORK / "_tce.c", WORK / "_tce.o"
    src.write_bytes(Path(path).read_bytes())
    if _clang("-c", "-O1", "-o", str(obj), str(src)).returncode != 0:
        return 1
    data = obj.read_bytes()
    data = data[:4] + b"\x00\x00\x00\x00" + data[8:]   # COFF TimeDateStamp: the one nondeterministic field
    print(hashlib.sha256(data).hexdigest())
    return 0


def rebuild() -> int:
    groups = [
        (conftest.FIFO_DLL, conftest.FIFO_DEPS, conftest._build_fifo_dll),
        (conftest.CLI_DLL, conftest.CLI_DEPS, conftest._build_cli_dll),
        (conftest.MEMSAFE_DLL, conftest.MEMSAFE_DEPS, conftest._build_memsafe_dll),
    ]
    for dll, deps, build in groups:
        if conftest._needs_build(dll, deps):
            try:
                build()
            except RuntimeError as ex:
                print(f"rebuild failed ({dll.name}): {str(ex)[:1500]}")
                return 1
    return 0


def main(argv) -> int:
    if len(argv) >= 2 and argv[0] == "check":
        return check(argv[1])
    if len(argv) >= 2 and argv[0] == "fingerprint":
        return fingerprint(argv[1])
    if argv[:1] == ["rebuild"]:
        return rebuild()
    print(__doc__)
    return 2


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
