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

#include "checksse42.h"
#include "common.h"
#include "crc32c.h"

/* Set at module loading time */
static crc_function crc_fn;

static
PyObject* crc32c_crc32(PyObject *self, PyObject *args) {
	Py_buffer pbin;
	unsigned char *bin_data = NULL;
	uint32_t crc = 0U, result;

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
	crc ^= 0xffffffff;
	result = crc_fn(crc, bin_data, pbin.len);
	result ^= 0xffffffff;

	PyBuffer_Release(&pbin);
	return PyLong_FromUnsignedLong(result);
}


static PyMethodDef CRC32CMethods[] = {
	{"crc32",   crc32c_crc32,   METH_VARARGS, "Calculate crc32c using Intel SSE4.2 instruction."},
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
	PyObject *m;

	char *sw_mode = getenv("CRC32C_SW_MODE");

	if (sw_mode != NULL && !strncmp(sw_mode, "1", 1)) {
		crc_fn = _crc32c_sw_slicing_by_8;
	}
	else if (_crc32c_intel_probe()) {
		crc_fn = _crc32c_hw_adler;
		crc32c_init_hw_adler();
	}
	else {
		PyErr_SetString(PyExc_ImportError, "\n"
		    "SSE4.2 extensions providing a crc32c hardware instruction are not\n"
		    "available in your processor. You can set the CRC32C_SW_MODE variable\n"
		    "to '1' to use a software implementation instead.\n");
		return MOD_VAL(NULL);
	}

	MOD_DEF(m, "crc32c", "wrapper for crc32c Intel instruction", CRC32CMethods);
	if (m == NULL) {
		return MOD_VAL(NULL);
	}
	return MOD_VAL(m);
}
