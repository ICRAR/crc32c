import subprocess
import sys

import pytest


@pytest.mark.calculates_crc32c
def test_benchmark() -> None:
    out = subprocess.check_output([sys.executable, "-m", "crc32c.benchmark"])
    assert b"crc32c ran at" in out
