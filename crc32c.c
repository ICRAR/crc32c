/*
 * crc32c checksum implementation (http://www.rfc-editor.org/rfc/rfc3385.txt)
 * based on the Intel CRC32 instruction
 * provided in the Intel SSE4.2 instruction set
 *
 * ICRAR - International Centre for Radio Astronomy Research
 * (c) UWA - The University of Western Australia, 2014
 * Copyright by UWA (in the framework of the ICRAR)
 * All rights reserved
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307  USA
 *
 */

#include "common.h"

// Support for x86 and x64 across several compilers, including MSVC
#ifdef _MSC_VER
#  include <nmmintrin.h>
#  ifdef _M_AMD64
#    define _CRC_INTRINSIC(x, crc) do { crc = _mm_crc32_u64(crc, (x)); } while (0)
     typedef uint64_t data_t ;
     typedef uint64_t crc_t ;
     const unsigned short word_size = 8;
#  else
#    define _CRC_INTRINSIC(x, crc) do { crc = _mm_crc32_u32(crc, (x)); } while (0)
     typedef uint32_t data_t;
     typedef uint32_t crc_t ;
     const unsigned short word_size = 4;
#  endif
#  define _CRC_INTRINSIC_U8(x, crc) do { crc = _mm_crc32_u8(crc, (x)); } while (0);
#else
#  if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
#    define REX_PRE "0x48, "
     const unsigned short word_size = 8;
     typedef uint64_t data_t;
#  else
#    define REX_PRE
     const unsigned short word_size = 4;
     typedef uint32_t data_t;
#  endif
     typedef uint32_t crc_t ;
#  define _CRC_INTRINSIC(x, crc) \
     do { __asm__ __volatile__( \
          ".byte 0xf2, " REX_PRE "0xf, 0x38, 0xf1, 0xf1;" \
          :"=S"(crc) \
          :"0"(crc), "c"((x)) \
     ); } while (0)
#  define _CRC_INTRINSIC_U8(x, crc) \
     do { __asm__ __volatile__( \
          ".byte 0xf2, 0xf, 0x38, 0xf0, 0xf1" \
          :"=S"(crc) \
          :"0"(crc), "c"((x)) \
     ); } while (0)
#endif

uint32_t _crc32c_intel(uint32_t crc, const unsigned char *data, unsigned long length)
{
	unsigned int words = length / word_size;
	unsigned int remainder = length % word_size;
	const data_t *ptr = (const data_t *)data;
	crc_t _crc = crc;

	for(; words; words--, ptr++) {
		_CRC_INTRINSIC(*ptr, _crc);
	}
	data = (unsigned const char *)ptr;
	crc = (uint32_t)_crc;
	for(; remainder; remainder--, data++) {
		_CRC_INTRINSIC_U8(*data, crc);
	}
	return crc;
}