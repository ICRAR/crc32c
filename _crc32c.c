/*
 *   This module provides crc32c checksum (http://www.rfc-editor.org/rfc/rfc3385.txt)
 *   based on the Intel CRC32 instruction
 *   provided in the Intel SSE4.2 instruction set
 *
 *   If SSE4.2 is not supported on the platform, this module will not be able to get installed
 *
 *    ICRAR - International Centre for Radio Astronomy Research
 *    (c) UWA - The University of Western Australia, 2014
 *    Copyright by UWA (in the framework of the ICRAR)
 *    All rights reserved
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 2.1 of the License, or (at your option) any later version.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA 02111-1307  USA
 *
 */

#include <Python.h>
#include <stdint.h>

#include "checksse42.h"
#include "consume.h"
#include "crc32c.h"

/* Python 3 doesn't have PyInt anymore */
#if PY_MAJOR_VERSION >= 3
	#define PyInt_FromLong PyLong_FromLong
#endif

static
PyObject* crc32c_consume(PyObject *self, PyObject *args) {

	int fd_in, fd_out;
	unsigned long buffsize;
	unsigned long total;
	unsigned short crc_type;
	uint32_t crc;
	int stat;
	float timeout;
	unsigned long crc_time = 0;
	unsigned long write_time = 0;
	Py_buffer pbin;
	unsigned char *buff_data;

	if (!PyArg_ParseTuple(args, "is*ifkkH", &fd_in, &pbin, &fd_out, &timeout, &buffsize, &total, &crc_type) )
		return NULL;

	buff_data = pbin.buf;
	crc = _crc32c_intel(0U, buff_data, pbin.len);
	stat = write(fd_out, buff_data, pbin.len);
	if( stat == -1 || stat != pbin.len ) {
		PyBuffer_Release(&pbin);
		char *error = strerror(errno);
		char *fmt = "Error while writing initial data: %s";
		char *msg = (char *)malloc(strlen(error) + strlen(fmt) - 1);
		sprintf(msg, fmt, error);
		PyErr_SetString(PyExc_Exception, msg);
		free(msg);
		return NULL;
	}
	total -= pbin.len;
	PyBuffer_Release(&pbin);

	Py_BEGIN_ALLOW_THREADS
	stat = _crc32c_read_crc_write(fd_in, fd_out, timeout, buffsize, total, crc_type, &crc, &crc_time, &write_time);
	Py_END_ALLOW_THREADS

	if( stat ) {
		stat = -1*stat - 1;
		char *error = strerror(errno);
		char *action[] = {"preparing to loop", "reading", "writing", "completing writing"};
		char *fmt = "Error while %s: %s";
		char *msg = (char *)malloc(strlen(error) + strlen(fmt) + strlen(action[stat]) - 1);
		sprintf(msg, fmt, action[stat], error);
		PyErr_SetString(PyExc_Exception, msg);
		free(msg);
		return NULL;
	}


	PyObject* res = PyTuple_New(3);
	PyTuple_SetItem(res, 0, PyInt_FromLong(crc));
	PyTuple_SetItem(res, 1, PyInt_FromLong(crc_time));
	PyTuple_SetItem(res, 2, PyInt_FromLong(write_time));
	return res;
}

static
PyObject* crc32c_crc32(PyObject *self, PyObject *args) {
	Py_buffer pbin;
	unsigned char *bin_data = NULL;
	uint32_t crc = 0U;      /* initial value of CRC for getting input */

	/* In python 3 we accept only bytes-like objects */
	const char *format =
#if PY_MAJOR_VERSION >= 3
	"y*"
#else
	"s*"
#endif
	"|I:crc32";

	if (!PyArg_ParseTuple(args, format, &pbin, &crc) )
		return NULL;

	bin_data = pbin.buf;
	uint32_t result = _crc32c_intel(crc, bin_data, pbin.len);

	PyBuffer_Release(&pbin);
	return PyInt_FromLong(result);
}


static PyMethodDef CRC32CMethods[] = {
	{"crc32",   crc32c_crc32,   METH_VARARGS, "Calculate crc32c using Intel SSE4.2 instruction."},
	{"consume", crc32c_consume, METH_VARARGS, "read/crc/write loop using Intel SSE4.2 instruction"},
	{NULL, NULL, 0, NULL}        /* Sentinel */
};

/* Support for Python 2/3 */
#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef moduledef = {PyModuleDef_HEAD_INIT, "crc32c", "wrapper for crc32c Intel instruction", -1, CRC32CMethods};
	#define MOD_INIT(name) PyMODINIT_FUNC PyInit_##name(void)
	#define MOD_DEF(m, name, doc, methods) \
		m = PyModule_Create(&moduledef);
	#define MOD_VAL(v) v
#else
	#define MOD_INIT(name) PyMODINIT_FUNC init##name(void)
	#define MOD_DEF(m, name, doc, methods) \
		m = Py_InitModule3(name, methods, doc);
	#define MOD_VAL(v)
#endif

MOD_INIT(crc32c)
{
	if( !_crc32c_intel_probe() ) {
		PyErr_SetString(PyExc_ImportError, "crc32c is not available in your processor");
		return MOD_VAL(NULL);
	}

	PyObject *m;
	MOD_DEF(m, "crc32c", "wrapper for crc32c Intel instruction", CRC32CMethods);
	if (m == NULL) {
		return MOD_VAL(NULL);
	}
	return MOD_VAL(m);
}
