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

#include <stdint.h>

#ifdef __WORDSIZE
#define BITS_PER_LONG	__WORDSIZE
#else
#define BITS_PER_LONG	32
#endif

#if BITS_PER_LONG == 64
#define REX_PRE "0x48, "
#define SCALE_F 8
#else
#define REX_PRE
#define SCALE_F 4
#endif

static inline
uint32_t crc32c_intel_le_hw_byte(uint32_t crc, unsigned const char *data,
										unsigned long length) {
	while (length--) {
		__asm__ __volatile__(
			".byte 0xf2, 0xf, 0x38, 0xf0, 0xf1"
			:"=S"(crc)
			:"0"(crc), "c"(*data)
		);
		data++;
	}

	return crc;
}

uint32_t _crc32c_intel(uint32_t crc, unsigned const char *data, unsigned long length) {
	unsigned int iquotient = length / SCALE_F;
	unsigned int iremainder = length % SCALE_F;

#if BITS_PER_LONG == 64
    uint64_t *ptmp = (uint64_t *) data;
#else
    uint32_t *ptmp = (uint32_t *) data;
#endif

	while (iquotient--) {
		__asm__ __volatile__(
			".byte 0xf2, " REX_PRE "0xf, 0x38, 0xf1, 0xf1;"
			:"=S"(crc)
			:"0"(crc), "c"(*ptmp)
		);
		ptmp++;
	}

	if (iremainder)
		crc = crc32c_intel_le_hw_byte(crc, (unsigned char *)ptmp,
				 iremainder);

	return crc;
}
