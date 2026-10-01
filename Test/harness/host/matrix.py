"""The configuration matrix of the C library: its vocabulary, its fragments, its files.

Decision 13 (wiki, Design/ConfigurationMatrix.md): every alx file is compiled in every row of
Test/config/matrix.toml, and the union of the rows compiles every line of alx code. A row is
<ID>_<toolchain>_<platform>_<features>_<target>_<library>, every word one fragment
Test/config/matrix/<axis>/<word>.h of #define and #undef lines over alxConfig_TEMPLATE.h.

This module states what every word means, writes the fragments from the template and wires the
run to this machine's compilers; the nox session `matrix` calls run():

    python -m harness.host.matrix          rewrite every fragment from the template

The framework test (tests/framework/test_matrix.py) fails while a committed fragment differs from
what this module writes, so a module added to the template reaches the matrix the day it is added.
The rows themselves are data, allocated once and never renumbered: matrix.toml is edited by hand.
The mechanism - synthesis, compile, coverage, report - is the Python library's alx.verify.configs.

Two choices of the compile, both recorded for TV in the wiki's morning report 4:
* -Wno-unused-parameter, as in the library's own warning set (alx.verify.host_build.WARNINGS): a
  backend stub that a configuration leaves without work is the library's normal shape.
* The host rows compile for a 32-bit PC (i686), whose pointers and integers have a target's widths:
  every Auralix target is 32-bit, and target code casting a uint32_t address to a pointer is right
  there; the 64-bit PC is the host lanes' question, over the portable modules they build.
"""

from __future__ import annotations

import re
import sys
from pathlib import Path
from typing import TYPE_CHECKING

from alx.verify import configs
from alx.verify import host_build as hb
from alx.verify.evidence import git_head

if TYPE_CHECKING:
    from collections.abc import Callable, Sequence

TEST = Path(__file__).resolve().parents[2]
CLIB = TEST.parent
CONFIG = TEST / "config"
MATRIX_FILE = CONFIG / "matrix.toml"
FRAGMENTS = CONFIG / "matrix"
TEMPLATE = CLIB / "alxConfig_TEMPLATE.h"
SHIM = TEST / "host" / "shim"

# The library's own include folders, the ones a product's build lists: the configuration folder of
# the row comes before them, the fakes of the platform after them.
INCLUDES = [
    CLIB, CLIB / "Ext", CLIB / "Mcu", CLIB / "Mcu" / "McuStm32",
    CLIB / "Mcu" / "McuLpc" / "McuLpc17xx", CLIB / "Mcu" / "McuLpc" / "McuLpc55S6x",
    CLIB / "Mcu" / "McuLpc" / "McuLpc80x", CLIB / "Mcu" / "McuSam", CLIB / "Mcu" / "McuZephyr",
    CLIB / "FatFs", CLIB / "Usbh", CLIB / "mcuboot", CLIB / "mcuboot" / "mcuboot_config",
]

# The headers a product supplies, written into the row's configuration folder as the library's own
# templates of them - so the template is what gets compiled and measured.
ALIASES = {
    "alxConfigUsr.h": CLIB / "alxConfigUsr_TEMPLATE.h",
    "alxBoot_GENERATED.h": CLIB / "alxBoot_GENERATED_TEMPLATE.h",
    "alxBootMetadata_GENERATED.h": CLIB / "alxBootMetadata_GENERATED_TEMPLATE.h",
    "ffconf_usr.h": CLIB / "FatFs" / "ffconf_usr_TEMPLATE.h",
    "usbh_conf_usr.h": CLIB / "Usbh" / "usbh_conf_usr_TEMPLATE.h",
    "mcuboot_config_usr.h": CLIB / "mcuboot" / "mcuboot_config" / "mcuboot_config_usr_TEMPLATE.h",
}

# Not Auralix's, so neither compiled nor measured: ChaN's FatFs (its configuration template is
# Auralix's and is measured), and the mcuboot configuration header taken over from mcuboot.
VENDOR = {"FatFs/ff.c", "FatFs/ffsystem.c", "FatFs/ffunicode.c", "FatFs/ff.h", "FatFs/ffconf.h",
          "FatFs/diskio.h", "mcuboot/mcuboot_config/mcuboot_config.h"}
_SKIP_DIRS = {"Test", "Doc", ".git", "__pycache__"}


def _library_files(suffixes: tuple[str, ...]) -> list[Path]:
    found = []
    for path in sorted(CLIB.rglob("*")):
        rel = path.relative_to(CLIB)
        if _SKIP_DIRS & set(rel.parts) or rel.as_posix() in VENDOR:
            continue
        if path.is_file() and path.suffix in suffixes:
            found.append(path)
    return found


def sources() -> list[Path]:
    """Every C file Auralix wrote, and the headers only others include: alxInc.h (products), mcuboot_logging.h (MCUboot)."""
    return [*_library_files((".c",)), CLIB / "alxInc.h", CLIB / "mcuboot" / "mcuboot_config" / "mcuboot_logging.h"]


def measured() -> list[Path]:
    """Every C file and header Auralix wrote: the files whose every line some row must compile."""
    return _library_files((".c", ".h"))


# ---- the template ------------------------------------------------------------------------------
_DASH = "//------------------------------------------------------------------------------"
_DEFINE = re.compile(r"^(?://)?#define\s+([A-Za-z_][A-Za-z0-9_]*)")


def template_groups(text: str | None = None) -> dict[tuple[str, str], list[str]]:
    """The template's defines by (category, group), in file order; a category's own list is group ''.

    A category title is a double dashed line around its name, a group heading a single one; the
    defines below a heading belong to it, the defines straight under a category title to ''.
    """
    lines = (TEMPLATE.read_text(encoding="utf-8") if text is None else text).splitlines()
    groups: dict[tuple[str, str], list[str]] = {}
    category = group = ""
    i = 0
    while i < len(lines):
        if lines[i] == _DASH and i + 4 < len(lines) and lines[i + 1] == _DASH and lines[i + 3] == _DASH:
            category, group = lines[i + 2][3:], ""
            i += 5
            continue
        if lines[i] == _DASH and i + 2 < len(lines) and lines[i + 2] == _DASH:
            group = lines[i + 1][3:]
            i += 3
            continue
        m = _DEFINE.match(lines[i])
        if m and category:
            names = groups.setdefault((category, group), [])
            if m.group(1) not in names:
                names.append(m.group(1))
        i += 1
    return groups


# ---- the vocabulary ----------------------------------------------------------------------------
STM32 = ["stm32f0", "stm32f1", "stm32f4", "stm32f7", "stm32g4", "stm32l0", "stm32l4", "stm32u5"]
OTHER = ["lpc17xx", "lpc55s6x", "lpc80x", "lpc84x", "sam", "ra", "zephyr"]
TARGET_PLATFORMS = [*STM32, *OTHER]
# A platform's variant states the platform's switches as the platform does; its table in matrix.toml
# gives what differs - the part, or the vendor's own configuration - to reach the lines the
# platform's one row set cannot.
PLATFORM_VARIANTS = {
    "zephyr-kconfig": ("zephyr", "with the Kconfig options the backends test: thread names, the 64-bit cycle counter, "
                                 "configurable ADC inputs"),
}
FEATURE_SWITCHES = ["ALX_CMSIS_DSP", "ALX_FATFS", "ALX_FREE_RTOS", "ALX_FREE_RTOS_CELLULAR",
                    "ALX_LFS", "ALX_LW_IP", "ALX_MBEDTLS", "ALX_MCU_BOOT", "ALX_USBH", "ALX_WIZNET"]
_ALL = {"ALX_CMSIS_DSP", "ALX_FATFS", "ALX_FREE_RTOS", "ALX_LFS", "ALX_MBEDTLS", "ALX_MCU_BOOT",
        "ALX_USBH", "ALX_WIZNET"}
# `all` and its variants: -cellular and -lwip take the network another way, -noos leaves FreeRTOS out
# (the bare-metal and Zephyr branches), -nomcuboot leaves MCUboot out - its flash backend is the STM32
# HAL's (mcuboot/flash_map_backend), so only the STM32 rows build a boot role.
_VARIANTS = {
    "": ("the network through Wiznet", set(), set()),
    "-cellular": ("the network through the FreeRTOS cellular interface", {"ALX_WIZNET"}, {"ALX_FREE_RTOS_CELLULAR"}),
    "-lwip": ("the network through lwIP", {"ALX_WIZNET"}, {"ALX_LW_IP"}),
    "-noos": ("no FreeRTOS: the bare-metal and Zephyr branches", {"ALX_FREE_RTOS"}, set()),
}
FEATURES = {"none": ("every feature off", set())}
for _mcuboot in ("", "-nomcuboot"):
    for _suffix, (_what, _off, _on) in _VARIANTS.items():
        _out = _off | ({"ALX_MCU_BOOT"} if _mcuboot else set())
        FEATURES[f"all{_suffix}{_mcuboot}"] = (
            f"every feature on; {_what}" + ("; no MCUboot" if _mcuboot else ""), (_ALL - _out) | _on)
_MEMORY_MAP = ["ALX_MCU_BOOT_FLASH_DEVICE_ID", "ALX_MCU_BOOT_FLASH_SECTOR_SIZE",
               "ALX_MCU_BOOT_BOOTLOADER_SIZE", "ALX_MCU_BOOT_IMAGE_SIZE", "ALX_MCU_BOOT_BOOTLOADER_OFFSET",
               "ALX_MCU_BOOT_IMAGE_PRIMARY_OFFSET", "ALX_MCU_BOOT_IMAGE_SECONDARY_OFFSET"]
_BUILD = ["ALX_BUILD_CONFIG_DEBUG", "ALX_BUILD_CONFIG_FW_UP", "ALX_BUILD_CONFIG_NO_BOOT"]
TARGETS = {
    "none": ("no boot role and no build configuration: a product without the bootloader", None, None),
    "app-debug": ("the application (boot role A), debug build", "ALX_BUILD_CONFIG_DEBUG", "A"),
    "app-fwup": ("the application (boot role A), firmware-update build", "ALX_BUILD_CONFIG_FW_UP", "A"),
    "app-noboot": ("the application (boot role A) built to run without the bootloader",
                   "ALX_BUILD_CONFIG_NO_BOOT", "A"),
    "boot-debug": ("the bootloader (boot role B), debug build", "ALX_BUILD_CONFIG_DEBUG", "B"),
}
LEVELS = {"off": "ALX_TRACE_LEVEL_OFF", "inf": "ALX_TRACE_LEVEL_INF", "vrb": "ALX_TRACE_LEVEL_VRB"}
ASSERTS = {"none": None, "bkpt": "BKPT", "trace": "TRACE", "rst": "RST"}
# The Module - Options that select code. ALX_INA228 and ALX_INA238 are one choice: the template
# ships INA238, the debug options take INA228.
# The two VRB words compile every trace and every assert with its arguments; only their rows count
# for coverage (matrix.toml, [coverage]), because an elided macro drops what it was given.
LIBRARY = {
    "trace-vrb-assert-bkpt-opt-none": ("every trace at VRB and BKPT asserts: every argument compiled",
                                       "vrb", "bkpt", "none"),
    "trace-vrb-assert-bkpt-opt-dbg": ("every trace at VRB, BKPT asserts, every module option on",
                                      "vrb", "bkpt", "dbg"),
    "trace-inf-assert-rst-opt-none": ("the template as shipped", "inf", "rst", "none"),
    "trace-inf-assert-trace-opt-none": ("TRACE asserts", "inf", "trace", "none"),
    "trace-off-assert-none-opt-none": ("every trace and every assert elided", "off", "none", "none"),
}

_HEADER = ("// {axis}: {word} - {what}.\n"
           "// Written by harness.host.matrix from alxConfig_TEMPLATE.h; do not edit, run\n"
           "// `python -m harness.host.matrix` from Test/ instead.\n")


def _fragment(axis: str, word: str, what: str, lines: list[str]) -> str:
    return _HEADER.format(axis=axis.capitalize(), word=word, what=what) + "".join(f"{x}\n" for x in lines)


def fragments(text: str | None = None) -> dict[str, str]:
    """Every fragment's relative path and text, from the template."""
    groups = template_groups(text)
    out: dict[str, str] = {}
    for word, what in (("gcc", "arm-none-eabi-gcc"), ("clang", "clang")):
        out[f"toolchain/{word}.h"] = _fragment(
            "toolchain", word, f"{what}; the template's minimum versions hold, nothing changes", [])
    platform_names = groups[("Platform", "Platform")]

    def platform_lines(word: str) -> list[str]:
        switch = f"ALX_{word.upper()}"
        return [f"#define {name}" for name in platform_names if name == switch or name.startswith(f"{switch}_")]

    for word in TARGET_PLATFORMS:
        out[f"platform/{word}.h"] = _fragment(
            "platform", word, f"ALX_{word.upper()}, with the minimum versions the template states", platform_lines(word))
    for word, (base, what) in PLATFORM_VARIANTS.items():
        out[f"platform/{word}.h"] = _fragment("platform", word, f"ALX_{base.upper()} {what}", platform_lines(base))
    out["platform/host.h"] = _fragment(
        "platform", "host", "no target: no Newlib and no CMSIS core on the PC",
        ["#undef ALX_NEWLIB_NANO", "#undef ALX_CMSIS_CORE", "#undef ALX_CMSIS_CORE_VER_REQUIRED"])
    for word, (what, on) in FEATURES.items():
        lines = [f"#define {s}" if s in on else f"#undef {s}" for s in FEATURE_SWITCHES]
        out[f"features/{word}.h"] = _fragment("features", word, what, lines)
    for word, (what, build, role) in TARGETS.items():
        lines = [f"#define {b}" if b == build else f"#undef {b}" for b in _BUILD]
        if role is None:
            lines += ["#undef ALX_BOOT_A", "#undef ALX_BOOT_A_PRE_COMP_BOOT_HDR_FILE",
                      "#undef ALX_BOOT_B", "#undef ALX_BOOT_B_ID_FW_ARTF", "#undef ALX_BOOT_B_ID_FW_NAME"]
            lines += [f"#undef {m}" for m in _MEMORY_MAP]
            out[f"target/{word}.h"] = _fragment("target", word, what, lines)
            continue
        if role == "A":
            lines += ["#define ALX_BOOT_A", "#define ALX_BOOT_A_PRE_COMP_BOOT_HDR_FILE",
                      "#undef ALX_BOOT_B", "#undef ALX_BOOT_B_ID_FW_ARTF", "#undef ALX_BOOT_B_ID_FW_NAME"]
        else:
            lines += ["#undef ALX_BOOT_A", "#undef ALX_BOOT_A_PRE_COMP_BOOT_HDR_FILE",
                      "#define ALX_BOOT_B", "#define ALX_BOOT_B_ID_FW_ARTF", "#define ALX_BOOT_B_ID_FW_NAME"]
        lines += [f"#define {m}" for m in _MEMORY_MAP]
        out[f"target/{word}.h"] = _fragment("target", word, what, lines)
    traces = groups[("Library", "Module - Trace")]
    forms = {form: groups[("Library", f"Module - Assert - {form}")] for form in ("BKPT", "TRACE", "RST")}
    options = groups[("Library", "Module - Options")]
    for word, (what, level, form, opt) in LIBRARY.items():
        lines = [f"#define ALX_TRACE_LEVEL {LEVELS[level]}"]
        lines += [f"#undef {t}" if level == "off" else f"#define {t}" for t in traces]
        for name, switches in forms.items():
            lines += [f"#define {s}" if ASSERTS[form] == name else f"#undef {s}" for s in switches]
        if opt == "dbg":
            lines += [f"#undef {o}" if o == "ALX_INA238" else f"#define {o}" for o in options]
        else:
            lines += [f"#define {o}" if o == "ALX_INA238" else f"#undef {o}" for o in options]
        out[f"library/{word}.h"] = _fragment("library", word, what, lines)
    return out


def write() -> list[str]:
    """Rewrite every fragment from the template; return the ones that changed."""
    changed = []
    wanted = fragments()
    for rel, text in wanted.items():
        path = FRAGMENTS / rel
        if not path.is_file() or path.read_text(encoding="ascii") != text:
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(text, encoding="ascii", newline="\n")
            changed.append(rel)
    for path in sorted(FRAGMENTS.rglob("*.h")):
        rel = path.relative_to(FRAGMENTS).as_posix()
        if rel not in wanted:
            path.unlink()
            changed.append(f"{rel} (removed)")
    return changed


# ---- the run --------------------------------------------------------------------------------------
WARNINGS = ("-Wall", "-Wextra", "-Wformat=2", "-Wno-unused-parameter", "-Werror")
OWN = ("Alx", "ALX_", "alx")   # an undeclared name with these prefixes is the library's finding, not a fake's


def compilers(tc: hb.Toolchain) -> dict[str, configs.Compiler]:
    """The two toolchain words on this machine: arm-none-eabi-gcc, and clang for the target and the PC.

    clang reads the target C library out of arm-gcc's own tree, the Newlib a product links.
    """
    armgcc = tc.armgcc()
    newlib = armgcc.parent.parent / "arm-none-eabi" / "include"
    return {
        "gcc": configs.Compiler("gcc", armgcc, diagnostics=configs.GCC_DIAGNOSTICS, warnings=WARNINGS),
        "clang": configs.Compiler(
            "clang", tc.llvm("clang"),
            target_flags=("--target=arm-none-eabi", f"-isystem{newlib}"),
            host_flags=("--target=i686-pc-windows-msvc", hb.CRT_DEFINE),
            diagnostics=configs.CLANG_DIAGNOSTICS, warnings=WARNINGS, env=tc.environment()),
    }


def matrix(tc: hb.Toolchain, heads: dict[str, str] | None = None) -> configs.Matrix:
    """The C library's matrix: the template, the fragments, every alx file, the compilers here."""
    return configs.Matrix(
        root=CLIB, base=TEMPLATE, config_name="alxConfig.h", fragments=FRAGMENTS,
        matrix=configs.load_matrix(MATRIX_FILE, TEST), sources=sources(), measured=measured(),
        includes=INCLUDES, compilers=compilers(tc), aliases=ALIASES, own=OWN, heads=heads or {})


def run(out: Path, only: Sequence[str] = (), log: Callable[[str], None] | None = None) -> dict:
    """Run the rows (all, or those whose id or name starts with one of `only`) into `out`."""
    return configs.run_matrix(matrix(hb.Toolchain(), {"c-lib": git_head(CLIB)}), out, only=only, log=log)


if __name__ == "__main__":
    for line in write():
        sys.stdout.write(f"{line}\n")
