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

import os
import struct
import sys
import unittest

try:
    import crc32c
    from crc32c import crc32
    sw_mode = os.environ.get('CRC32C_SW_MODE')
    if sw_mode == 'none' and not crc32c.hardware_based:
        raise RuntimeError('"none" should force hardware support')
    elif sw_mode == 'force' and crc32c.hardware_based:
        raise RuntimeError('"force" should force software support')
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
class TestMisc(unittest.TestCase):

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


numbers1 = ('Numbers1', b'123456789', 0xe3069283)
numbers2 = ('Numbers2', b'23456789', 0xBFE92A83)
numbers3 = ('Numbers3', b'1234567890', 0xf3dbd4fe)
phrase = ('Phrase', b'The quick brown fox jumps over the lazy dog', 0x22620404)
long_phrase = ('LongPhrase', (b"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nunc omni virtuti vitium contrario nomine opponitur. "
b"Conferam tecum, quam cuique verso rem subicias; Te ipsum, dignissimum maioribus tuis, voluptasne induxit, ut adolescentulus eriperes "
b"P. Conclusum est enim contra Cyrenaicos satis acute, nihil ad Epicurum. Duo Reges: constructio interrete. Tum Torquatus: Prorsus, inquit, assentior;\n"
b"Quando enim Socrates, qui parens philosophiae iure dici potest, quicquam tale fecit? Sed quid sentiat, non videtis. Haec quo modo conveniant, non "
b"sane intellego. Sed ille, ut dixi, vitiose. Dic in quovis conventu te omnia facere, ne doleas. Quod si ita se habeat, non possit beatam praestare "
b"vitam sapientia. Quis suae urbis conservatorem Codrum, quis Erechthei filias non maxime laudat? Primum divisit ineleganter; Huic mori optimum esse "
b"propter desperationem sapientiae, illi propter spem vivere."), 0xfcb7575a)

class Crc32cChecks(object):
    def test_all(self):
        self.assertEqual(self.checksum, crc32(self.val))
    def test_piece_by_piece(self):
        c = 0
        for x in as_individual_bytes(self.val):
            c = crc32(x, c)
        self.assertEqual(self.checksum, c)

# Generate the actual unittest classes for each of the testing values
if crc32 is not None:
    for name, val, checksum in (numbers1, numbers2, numbers3, phrase, long_phrase):
        classname = 'Test%s' % name
        locals()[classname] = type(classname, (unittest.TestCase, Crc32cChecks), {'val': val, 'checksum': checksum})