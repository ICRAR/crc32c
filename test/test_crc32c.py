#
#    ICRAR - International Centre for Radio Astronomy Research
#    (c) UWA - The University of Western Australia, 2018
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

from __future__ import annotations

import os
import struct
import warnings
from typing import Generator, List, NamedTuple

import pytest

import crc32c


def test_software_mode(crc32c_is_available: bool) -> None:
    """Check that the CRC32C_SW_MODE has intended consequences"""
    sw_mode = os.environ.get("CRC32C_SW_MODE")
    if sw_mode == "force":
        assert not crc32c.hardware_based
    if sw_mode == "none" and crc32c_is_available:
        assert crc32c.hardware_based


def ulonglong_as_bytes(x: int) -> bytes:
    return struct.pack("<Q", x)


def uint_as_bytes(x: int) -> bytes:
    return struct.pack("<I", x)


def ushort_as_bytes(x: int) -> bytes:
    return struct.pack("<H", x)


def uchar_as_bytes(c: int) -> bytes:
    return struct.pack("<B", c)


def batched(x: bytes, size: int) -> Generator[bytes, None, None]:
    length = len(x)
    for i in range(0, length, size):
        yield x[i : min(i + size, length)]


def as_individual_bytes(x: bytes) -> Generator[bytes, None, None]:
    for b in x:
        yield bytes([b])


@pytest.mark.calculates_crc32c
class TestMisc:

    def test_zero(self) -> None:
        assert 0 == crc32c.crc32c(b"")

    def test_keyword(self) -> None:
        assert 10 == crc32c.crc32c(b"", value=10)

    def test_gil_behaviour(self) -> None:
        def _test(data: bytes) -> None:
            expected = crc32c.crc32c(data)
            assert expected == crc32c.crc32c(data, gil_release_mode=-1)
            assert expected == crc32c.crc32c(data, gil_release_mode=0)
            assert expected == crc32c.crc32c(data, gil_release_mode=1)

        _test(b"this_doesnt_release_the_gil_by_default")
        _test(b"this_releases_the_gil_by_default" * 1024 * 1024)

    def test_crc32_deprecated(self) -> None:
        with warnings.catch_warnings(record=True) as warns:
            crc32c.crc32(b"")
        assert 1 == len(warns)
        with warnings.catch_warnings(record=True) as warns:
            crc32c.crc32c(b"")
        assert 0 == len(warns)

    def test_msvc_examples(self) -> None:
        # Examples taken from MSVC's online examples.
        # Values are not xor'd in the examples though, so we do it here
        max32 = 0xFFFFFFFF

        def assert_msvc_vals(b: bytes, crc: int, expected_crc: int) -> None:
            assert expected_crc ^ max32 == crc32c.crc32c(b, crc ^ max32)

        assert_msvc_vals(uchar_as_bytes(100), 1, 1412925310)
        assert_msvc_vals(ushort_as_bytes(1000), 1, 3870914500)
        assert_msvc_vals(uint_as_bytes(50000), 1, 971731851)
        assert_msvc_vals(ulonglong_as_bytes(0x88889999EEEE3333), 0x5555AAAA, 0x16F57621)


class CRCTestValue(NamedTuple):
    name: str
    data: bytes
    crc: int


test_values: List[CRCTestValue] = [
    CRCTestValue("Numbers1", b"123456789", 0xE3069283),
    CRCTestValue("Numbers2", b"23456789", 0xBFE92A83),
    CRCTestValue("Phrase", b"The quick brown fox jumps over the lazy dog", 0x22620404),
    CRCTestValue(
        "LongPhrase",
        b"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nunc omni virtuti vitium contrario nomine opponitur. "
        b"Conferam tecum, quam cuique verso rem subicias; Te ipsum, dignissimum maioribus tuis, voluptasne induxit, ut adolescentulus eriperes "
        b"P. Conclusum est enim contra Cyrenaicos satis acute, nihil ad Epicurum. Duo Reges: constructio interrete. Tum Torquatus: Prorsus, inquit, assentior;\n"
        b"Quando enim Socrates, qui parens philosophiae iure dici potest, quicquam tale fecit? Sed quid sentiat, non videtis. Haec quo modo conveniant, non "
        b"sane intellego. Sed ille, ut dixi, vitiose. Dic in quovis conventu te omnia facere, ne doleas. Quod si ita se habeat, non possit beatam praestare "
        b"vitam sapientia. Quis suae urbis conservatorem Codrum, quis Erechthei filias non maxime laudat? Primum divisit ineleganter; Huic mori optimum esse "
        b"propter desperationem sapientiae, illi propter spem vivere.",
        0xFCB7575A,
    ),
    CRCTestValue("Empty", b"", 0x0),
]


@pytest.mark.calculates_crc32c
class TestCRC32CHash:
    def test_misc(self) -> None:
        crc32c_hash = crc32c.CRC32CHash()

        assert crc32c_hash.digest_size == 4
        assert crc32c_hash.name == "crc32c"
        assert len(crc32c_hash.digest()) == crc32c_hash.digest_size
        assert len(crc32c_hash.hexdigest()) == crc32c_hash.digest_size * 2

    def test_initial_value(self) -> None:
        crc32c_hash = crc32c.CRC32CHash()
        crc32c_hash.update(b"hello world")
        expected = crc32c_hash.digest()

        crc32c_hash = crc32c.CRC32CHash(b"hello")
        crc32c_hash.update(b" world")
        assert expected == crc32c_hash.digest()

    def test_copy(self) -> None:
        crc32c_hash = crc32c.CRC32CHash()
        crc32c_hash_copy = crc32c_hash.copy()

        assert crc32c_hash.digest() == crc32c_hash_copy.digest()
        assert crc32c_hash.hexdigest() == crc32c_hash_copy.hexdigest()
        assert id(crc32c_hash) != id(crc32c_hash_copy)

        crc32c_hash.update(b"1")
        assert crc32c_hash.digest() != crc32c_hash_copy.digest()
        assert crc32c_hash.hexdigest() != crc32c_hash_copy.hexdigest()

        crc32c_hash_copy.update(b"2")
        assert crc32c_hash.digest() != crc32c_hash_copy.digest()
        assert crc32c_hash.hexdigest() != crc32c_hash_copy.hexdigest()

    @pytest.mark.parametrize(
        "data,crc",
        [pytest.param(value.data, value.crc, id=value.name) for value in test_values],
    )
    class TestSpecificValues:
        @staticmethod
        def _check_values(crc32c_hash: crc32c.CRC32CHash, crc: int) -> None:
            assert crc32c_hash.checksum == crc
            assert int.from_bytes(crc32c_hash.digest(), "big") == crc
            assert len(crc32c_hash.digest()) == 4
            assert int(crc32c_hash.hexdigest(), 16) == crc
            assert len(crc32c_hash.hexdigest()) == 8

        def test_piece_by_piece(self, data: bytes, crc: int) -> None:
            crc32c_hash = crc32c.CRC32CHash()
            for x in as_individual_bytes(data):
                crc32c_hash.update(x)
            self._check_values(crc32c_hash, crc)

        def test_all(self, data: bytes, crc: int) -> None:
            self._check_values(crc32c.CRC32CHash(data), crc)


@pytest.mark.calculates_crc32c
@pytest.mark.parametrize(
    "data,checksum",
    [pytest.param(value.data, value.crc, id=value.name) for value in test_values],
)
class TestCRC32CValues:

    def test_all(self, data: bytes, checksum: int) -> None:
        assert checksum == crc32c.crc32c(data)

    def test_piece_by_piece(self, data: bytes, checksum: int) -> None:
        c = 0
        for x in as_individual_bytes(data):
            c = crc32c.crc32c(x, c)
        assert checksum == c

    def test_by_different_chunk_lenghts(self, data: bytes, checksum: int) -> None:
        for chunk_size in range(1, 33):
            c = 0
            for chunk in batched(data, chunk_size):
                c = crc32c.crc32c(bytes(chunk), c)
            assert checksum == c

    def test_by_different_memory_offsets(self, data: bytes, checksum: int) -> None:
        for offset in range(16):
            view = memoryview(data)
            c = crc32c.crc32c(view[0:offset])
            c = crc32c.crc32c(view[offset:], c)
            assert checksum == c, (
                "Invalid checksum when splitting at offset %d" % offset
            )
