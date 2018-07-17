/*
 *  Common macro definitions
 *
 *  ICRAR - International Centre for Radio Astronomy Research
 *  (c) UWA - The University of Western Australia, 2014
 *  Copyright by UWA (in the framework of the ICRAR)
 *  All rights reserved
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA 02111-1307  USA
 *
 */

#ifndef _COMMON_H_
#define _COMMON_H_

/* MSVC starting include stdint.h fairly recently */
#if !defined(_MSC_VER) || (MSC_VER >= 1800)
# include <stdint.h>
#else
# include <limits.h>
  typedef unsigned __int64 uint64_t;
# if ULONG_MAX == (0xffffffffUL)
   typedef unsigned long uint32_t;
# elif UINT_MAX == (0xffffffffUL)
   typedef unsigned int uint32_t;
# else
#  error "Unsupported platform"
# endif
#endif

/* Definition of size_t */
#include <stdlib.h>

typedef uint32_t (* crc_function)(uint32_t crc, unsigned const char *data, unsigned long length);

#endif // _COMMON_H_
