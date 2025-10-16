import subprocess
import sys
import typing

import pytest

from crc32c import crc32c


@pytest.mark.parametrize("filename", [__file__, sys.executable])
@pytest.mark.parametrize("opts", [["-s"], ["-N"], ["-M"], ["-b", str(1024 * 1024)]])
@pytest.mark.calculates_crc32c
def test_cli_outputs_checksum(filename: str, opts: typing.List[str]) -> None:
    with open(filename, "rb") as f:
        expected_checksum = crc32c(f.read())
    output = subprocess.check_output([sys.executable, "-m", "crc32c._cli", filename])
    checksum, _, _ = output.partition(b" ")
    assert expected_checksum == int(checksum, 16)
