"""Auralix C Library - the host toolchain of the lanes: where the tools are, and the MSVC build environment.

Tool locations are machine configuration, never repository content: an environment variable names the
instance on this machine (ALX_LLVM_DIR, ALX_ARMGCC, ALX_CPPCHECK), this module carries the defaults of the
reference bench. A lane that needs a tool fails when it is missing (FileNotFoundError naming the variable),
it never skips. The lookup itself is `alx.verify.lanes.tool` of the Auralix Python lib.

Windows today: clang-cl, llvm-cov, clang-tidy from ALX_LLVM_DIR; the MSVC headers and libraries reach
clang-cl through the environment vcvars64.bat produces (`vcvars_env()`, captured once per run). Linux is
the same set of functions with the tools taken from PATH and no vcvars (that branch is untested so far).
"""

import os
import subprocess
import sys
from functools import lru_cache
from pathlib import Path

from alx.verify.lanes import tool   # the tool at the environment variable, else the default; missing = FileNotFoundError

WINDOWS = sys.platform == "win32"


def llvm_dir() -> Path:
    return tool("ALX_LLVM_DIR", "C:/Program Files/LLVM/bin" if WINDOWS else "/usr/bin")


def _llvm(name: str) -> Path:
    exe = llvm_dir() / (f"{name}.exe" if WINDOWS else name)
    if not exe.exists():
        raise FileNotFoundError(f"tool not found: {exe} (install LLVM or set ALX_LLVM_DIR)")
    return exe


def clang() -> Path:
    return _llvm("clang")


def clang_cl() -> Path:
    """The compiler of the instrumented variants (sanitizer, coverage): clang-cl on Windows, clang elsewhere."""
    return _llvm("clang-cl" if WINDOWS else "clang")


def clang_tidy() -> Path:
    return _llvm("clang-tidy")


def llvm_profdata() -> Path:
    return _llvm("llvm-profdata")


def llvm_cov() -> Path:
    return _llvm("llvm-cov")


def armgcc() -> Path:
    """The analysis-only ARM toolchain (ANALYZE stage 3, -fanalyzer) - deliberately newer than the frozen
    per-project production toolchains: findings are source properties, keep the analyzer fresh."""
    default = "C:/SysGCC/arm-eabi-15-2-1/bin/arm-none-eabi-gcc.exe" if WINDOWS else "/usr/bin/arm-none-eabi-gcc"
    return tool("ALX_ARMGCC", default)


def cppcheck() -> Path:
    return tool("ALX_CPPCHECK", "C:/Program Files/Cppcheck/cppcheck.exe" if WINDOWS else "/usr/bin/cppcheck")


def vcvars() -> Path:
    """vcvars64.bat of the newest Visual Studio (vswhere), the build environment of every clang-cl lane."""
    vswhere = Path(os.environ["ProgramFiles(x86)"]) / "Microsoft Visual Studio" / "Installer" / "vswhere.exe"
    if not vswhere.exists():
        raise FileNotFoundError(f"vswhere not found: {vswhere} (Visual Studio 2022 with the C++ workload)")
    vs_path = subprocess.run([str(vswhere), "-latest", "-property", "installationPath"],
                             capture_output=True, text=True, check=True).stdout.strip()
    bat = Path(vs_path) / "VC" / "Auxiliary" / "Build" / "vcvars64.bat"
    if not bat.exists():
        raise FileNotFoundError(f"vcvars64.bat not found under {vs_path}")
    return bat


@lru_cache(maxsize=1)
def vcvars_env() -> dict:
    """The process environment after vcvars64.bat (INCLUDE, LIB, PATH, ...), captured once; Linux: as is."""
    env = dict(os.environ)
    if not WINDOWS:
        return env
    out = subprocess.run(f'"{vcvars()}" >nul 2>&1 && set', shell=True, capture_output=True, text=True, check=True).stdout
    for line in out.splitlines():
        key, sep, value = line.partition("=")
        if sep and key:
            env[key] = value
    return env


def clang_resource_dir() -> Path:
    return Path(subprocess.run([str(clang()), "-print-resource-dir"], capture_output=True, text=True,
                               check=True).stdout.strip())


def asan_runtime() -> Path:
    """The compiler's OWN ASan runtime DLL; it must sit next to the smoke exe to shadow MSVC's older copy."""
    sub = ("lib", "windows", "clang_rt.asan_dynamic-x86_64.dll") if WINDOWS else ("lib", "linux", "libclang_rt.asan-x86_64.so")
    return clang_resource_dir().joinpath(*sub)
