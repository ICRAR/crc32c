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

#if defined(WIN32) || defined(_WIN32) || defined(_MSC_VER) || defined(__MINGW32__)
#define _IS_WINDOWS
#else
#undef _IS_WINDOWS
#endif

#endif // _COMMON_H_
