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

import sys
import unittest
import struct

try:
    from crc32c import crc32
except ImportError:
    crc32 = None

def ulonglong_as_bytes(x):
    return struct.pack('<Q', x)
def uint_as_bytes(x):
    return struct.pack('<I', x)
def ushort_as_bytes(x):
    return struct.pack('<H', x)
def uchar_as_bytes(c):
    return struct.pack('<B', c)

if sys.version_info[0] == 2:
    def as_individual_bytes(x):
        return x
else:
    def as_individual_bytes(x):
        for b in x:
            yield bytes([b])

@unittest.skipIf(crc32 is None, 'no crc32c support in this platform')
class TestCrc32c(unittest.TestCase):

    check = 0xe3069283

    def test_all(self):
        self.assertEqual(self.check, crc32(b'123456789'))

    def test_piece_by_piece(self):
        c = crc32(b'1')
        for x in as_individual_bytes(b'23456789'):
            c = crc32(x, c)
        self.assertEqual(self.check, c)

    def test_zero(self):
        self.assertEqual(0, crc32(b''))

    def test_msvc_examples(self):
        # Examples taken from MSVC's online examples.
        # Values are not xor'd in the examples though, so we do it here
        max32 = 0xffffffff
        def assert_msvc_vals(b, crc, expected_crc):
            self.assertEqual(expected_crc ^ max32, crc32(b, crc ^ max32))
        assert_msvc_vals(uchar_as_bytes(100), 1, 1412925310)
        assert_msvc_vals(ushort_as_bytes(1000), 1, 3870914500)
        assert_msvc_vals(uint_as_bytes(50000), 1, 971731851)
        assert_msvc_vals(ulonglong_as_bytes(0x88889999EEEE3333), 0x5555AAAA, 0x16F57621)