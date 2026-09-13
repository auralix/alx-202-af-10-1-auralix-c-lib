"""Architectural dependency checks run in fresh processes, outside pytest fixture imports."""

import subprocess
import sys
from pathlib import Path


def test_ALX1564_P300_build_runner_does_not_import_access_or_fixture_application():
    result = subprocess.run(
        [sys.executable, "-c",
         "import sys; import noxfile; "
         "assert 'host_build' in sys.modules; "
         "assert 'host_harness' not in sys.modules; "
         "assert 'conftest' not in sys.modules"],
        cwd=Path(__file__).parent, capture_output=True, text=True, check=False,
    )
    assert result.returncode == 0, result.stdout + result.stderr
