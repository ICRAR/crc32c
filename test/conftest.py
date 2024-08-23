#
#    ICRAR - International Centre for Radio Astronomy Research
#    (c) UWA - The University of Western Australia, 2024
#    Copyright by UWA (in the framework of the ICRAR)
#    All rights reserved
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#    MA 02111-1307  USA
#

import time

import pytest

import crc32c


def _crc32c_is_available() -> bool:
    try:
        crc32c.crc32c(b"dummy")
        return True
    except RuntimeError:
        return False


@pytest.fixture
def crc32c_is_available() -> bool:
    return _crc32c_is_available()


@pytest.fixture(autouse=True)
def skip_if_crc32c_unavailable(
    request: pytest.FixtureRequest, crc32c_is_available: bool
) -> None:
    if request.node.get_closest_marker("calculates_crc32c") and not crc32c_is_available:
        pytest.skip("crc32c is not available on this platform")


def pytest_sessionstart(session: pytest.Session) -> None:
    print("crc32c is big endian? ", crc32c.big_endian)
    print("crc32c is hardware based? ", crc32c.hardware_based)
