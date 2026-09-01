"""ASCII gate: library sources must contain no byte > 127 (pure ASCII rule).

Scope: own code only - root *.c/*.h, Mcu/**, Test/*.c|*.h|*.py.
Vendor dirs (Ext, FatFs, mcuboot, Usbh) are third-party and excluded.
Usage: ascii_gate.py <clib-root>
"""
import pathlib
import sys

root = pathlib.Path(sys.argv[1])
files = (
    list(root.glob("*.c")) + list(root.glob("*.h"))
    + list(root.glob("Mcu/**/*.c")) + list(root.glob("Mcu/**/*.h"))
    + list(root.glob("Test/*.c")) + list(root.glob("Test/*.h")) + list(root.glob("Test/*.py"))
)
bad = []
for f in files:
    data = f.read_bytes()
    for i, b in enumerate(data):
        if b > 127:
            line = data[:i].count(b"\n") + 1
            bad.append(f"{f}:{line}: byte 0x{b:02X}")
            break  # first offender per file is enough
print(f"ASCII GATE: {'FAIL' if bad else 'PASS'} ({len(files)} files)")
for b in bad:
    print(" ", b)
sys.exit(1 if bad else 0)
