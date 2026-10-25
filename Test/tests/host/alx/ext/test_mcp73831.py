"""Auralix C Library - AlxMcp73831 on the host.

A single-cell charger that reports its state on one pin with three levels: STAT low while it
charges, high when the charge is complete, and floating when it is shut down (no input or no
battery). A plain digital read cannot see the third level, so the driver offers two read modes,
chosen at construction: a tri-state read that tells all three apart, and a binary read that
knows only charging and standby.

Proofs (ALX-1564, A92): P585-P588.
"""

import pytest


def test_ALX1564_P585_init_claims_the_status_pin_and_deinit_releases_it(mcp73831_lib):
    """The charger owns one pin and gives it back."""
    obj = mcp73831_lib.new(tri_state=True)
    assert mcp73831_lib.pin_counts(obj) == (1, 0)

    mcp73831_lib.deinit(obj)

    assert mcp73831_lib.pin_counts(obj) == (1, 1)


@pytest.mark.parametrize(("strap", "expected"), [
    ("Lo", "Charging"),
    ("Hi", "Standby"),
    ("HiZ", "Shutdown"),
    ("Undefined", "Err"),
])
def test_ALX1564_P586_a_tri_state_read_tells_all_three_levels_apart(mcp73831_lib, strap, expected):
    """Each level of the pin is one status, and a read that cannot decide is an error, not a guess."""
    obj = mcp73831_lib.new(tri_state=True)
    mcp73831_lib.strap(obj, strap)

    assert mcp73831_lib.status(obj) == expected


@pytest.mark.parametrize(("level", "expected"), [
    (False, "Charging"),
    (True, "Standby"),
])
def test_ALX1564_P587_a_binary_read_knows_only_charging_and_standby(mcp73831_lib, level, expected):
    """CHARACTERIZATION: with the plain read, a floating pin reads as whatever its pull makes it.

    So a shut-down charger is reported as standby (or charging) and never as shut down - the price
    of the cheaper read, and the reason the tri-state mode exists.
    """
    obj = mcp73831_lib.new(tri_state=False)
    mcp73831_lib.set_level(obj, level)

    assert mcp73831_lib.status(obj) == expected


def test_ALX1564_P588_the_constructor_decides_which_read_the_driver_makes(mcp73831_lib):
    """A tri-state charger reads only the tri-state, a binary one only the level.

    The fake keeps the two apart on one pin, so a driver that read the wrong one would answer with
    the other's status here.
    """
    tri = mcp73831_lib.new(tri_state=True)
    mcp73831_lib.set_level(tri, False)          # a plain read would say charging
    mcp73831_lib.strap(tri, "Hi")
    assert mcp73831_lib.status(tri) == "Standby"

    binary = mcp73831_lib.new(tri_state=False)
    mcp73831_lib.strap(binary, "HiZ")           # a tri-state read would say shutdown
    mcp73831_lib.set_level(binary, False)
    assert mcp73831_lib.status(binary) == "Charging"
