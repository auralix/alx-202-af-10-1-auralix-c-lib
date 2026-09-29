"""Architectural dependency checks run in fresh processes, outside pytest fixture imports.

And the verification root is laid out by the template, checked by the Python library's
alx.verify.layout: the rule is the library's, this repository only asks it.
"""

import subprocess
import sys
from pathlib import Path

from alx.verify import layout


def test_ALX1564_P339_the_verification_root_follows_the_template():
    assert layout.check(Path(__file__).resolve().parents[2], "c") == []


def test_ALX1564_P300_build_runner_does_not_import_access_or_fixture_application():
    result = subprocess.run(
        [sys.executable, "-c",
         "import sys; import noxfile; "
         "assert 'harness.build' in sys.modules; "
         "assert 'harness.access' not in sys.modules; "
         "assert 'conftest' not in sys.modules; assert 'tests.conftest' not in sys.modules"],
        cwd=Path(__file__).resolve().parents[2], capture_output=True, text=True, check=False,
    )
    assert result.returncode == 0, result.stdout + result.stderr
