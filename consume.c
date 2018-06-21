/*
 *  C implementation of a simple read+CRC+write loop using crc32c
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

#include "common.h"

#ifndef _IS_WINDOWS

#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include "crc32c.h"

int _crc32c_read_crc_write(int fd_in, int fd_out,
                           float in_timeout,
                           size_t buffsize, uint64_t total,
                           int crc_type, uint32_t* crc,
                           unsigned long *crc_time,
                           unsigned long *write_time)
{

	struct timeval start, end;
	int stat;

	uint64_t remainder = total;
	int orig_in_flags = fcntl(fd_in, F_GETFL);
	stat = fcntl(fd_in, F_SETFL, orig_in_flags & ~O_NONBLOCK );
	if( stat ) {
		return -1;
	}

	struct timeval original_to, to;
	socklen_t size = sizeof(to);
	to.tv_sec = (long)floor(in_timeout);
	to.tv_usec = (long)(in_timeout - to.tv_sec);
	stat = getsockopt(fd_in, SOL_SOCKET, SO_RCVTIMEO, &original_to, &size);
	if( stat ) {
		return -1;
	}
	size = sizeof(to);
	stat = setsockopt(fd_in, SOL_SOCKET, SO_RCVTIMEO, &to, size);
	if( stat ) {
		return -1;
	}

	stat = 0;
	void* tmp_buff = malloc(buffsize);
	if (tmp_buff == NULL) {
		return -1;
	}

	while (remainder) {

		size_t count = remainder >= buffsize ? buffsize : remainder;
		ssize_t readin = read(fd_in, tmp_buff, count);
		if (readin == -1 || readin == 0) {
			stat = -2;
			break;
		}
		remainder -= readin;

		gettimeofday(&start, NULL);

		ssize_t writeout = write(fd_out, tmp_buff, readin);
		if (writeout < readin) {
			stat = -3;
			break;
		}

		gettimeofday(&end, NULL);
		*write_time += (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

		/* Run the CRC32C and time it */
		gettimeofday(&start, NULL);
		*crc = _crc32c_intel(*crc, tmp_buff, readin);
		gettimeofday(&end, NULL);
		*crc_time += (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
	}

	// TODO: check these two
	fcntl(fd_in, F_SETFL, orig_in_flags);
	setsockopt(fd_in, SOL_SOCKET, SO_RCVTIMEO, &original_to, size);

	free(tmp_buff);
	return stat;
}
#endif // _IS_WINDOWS