/*
 *   This module provides crc32c checksum (http://www.rfc-editor.org/rfc/rfc3385.txt)
 *   based on the Intel CRC32 instruction
 *   provided in the Intel SSE4.2 instruction set
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

#include "checkarm.h"
#include "checksse42.h"
#include "common.h"
#include "crc32c.h"

#define MIN_BUFSIZE_FOR_AUTOMATIC_RELEASE 32 * 1024  /* threshold for GIL release is 32KiB */

/* Set at module loading time */
static crc_function crc_fn;
int is_big_endian;

static inline int crc32c_inline(uint32_t crc, unsigned char *bin_data, Py_ssize_t len) {
	int result;
	crc ^= 0xffffffff;
	result = crc_fn(crc, bin_data, len);
	result ^= 0xffffffff;
	return result;
}

static
PyObject* crc32c_crc32c(PyObject *self, PyObject *args, PyObject *kwargs) {
	Py_buffer pbin;
	unsigned char *bin_data = NULL;
	uint32_t crc = 0U, result;
	int gil_release_mode = -1;

	static char *kwlist[] = {"data", "value", "gil_release_mode", NULL};

	/* In python 3 we accept only bytes-like objects */
	const char *format ="y*|Ii:crc32";

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, format, kwlist, &pbin, &crc, &gil_release_mode) )
		return NULL;

	bin_data = pbin.buf;
	if ((gil_release_mode < 0 && pbin.len >= MIN_BUFSIZE_FOR_AUTOMATIC_RELEASE) || gil_release_mode >= 1) {
		Py_BEGIN_ALLOW_THREADS
		result = crc32c_inline(crc, bin_data, pbin.len);
		Py_END_ALLOW_THREADS
	} else {
		result = crc32c_inline(crc, bin_data, pbin.len);
	}

	PyBuffer_Release(&pbin);
	return PyLong_FromUnsignedLong(result);
}

static
PyObject *crc32c_crc32(PyObject *self, PyObject *args, PyObject *kwargs)
{
	if (PyErr_WarnEx(PyExc_DeprecationWarning,
	                 "crc32c.crc32 will be eventually removed, use crc32c.crc32c instead",
	                 1) == -1) {
		return NULL;
	}
	return crc32c_crc32c(self, args, kwargs);
}

/* The different values the SW mode preference can take */
enum crc32c_sw_mode {
	UNSPECIFIED,
	AUTO,
	FORCE,
	NONE
};

static enum crc32c_sw_mode get_sw_mode(void)
{
	char *sw_mode = getenv("CRC32C_SW_MODE");
	if (sw_mode == NULL) {
		return UNSPECIFIED;
	}
	else if (!strcmp(sw_mode, "auto")) {
		return AUTO;
	}
	else if (!strcmp(sw_mode, "force")) {
		return FORCE;
	}
	else if (!strcmp(sw_mode, "none")) {
		return NONE;
	}
	return UNSPECIFIED;
}

static PyMethodDef CRC32CMethods[] = {
	{"crc32",   (PyCFunction)crc32c_crc32,   METH_VARARGS | METH_KEYWORDS, "Calculate crc32c incrementally (deprecated)"},
	{"crc32c",  (PyCFunction)crc32c_crc32c,  METH_VARARGS | METH_KEYWORDS, "Calculate crc32c incrementally"},
	{NULL, NULL, 0, NULL}        /* Sentinel */
};

static const char *import_error_msg = "\n\n"
"Hardware extensions providing a crc32c hardware instruction are not available in\n"
"your processor. This package comes with a software implementation, but this\n"
"support has been opted out because the CRC32C_SW_MODE environment variable is\n"
"set to \"none\". CRC32C_SW_MODE can take one of the following values:\n"
" * If unset: use the software implementation if no hardware support is found\n"
" * 'auto': as above, but will eventually be discontinued\n"
" * 'force': use software implementation regardless of hardware support.\n"
" * 'none': fail if no hardware support is found (this error).\n";

static struct PyModuleDef moduledef = {PyModuleDef_HEAD_INIT, "_crc32c", "crc32c implementation in hardware and software", -1, CRC32CMethods};

PyMODINIT_FUNC PyInit__crc32c(void)
{
	PyObject *m;
	PyObject *hardware_based;
	enum crc32c_sw_mode sw_mode;
	const uint32_t n = 1;

	sw_mode = get_sw_mode();
	crc_fn = NULL;
	if (sw_mode == FORCE) {
		crc_fn = _crc32c_sw_slicing_by_8;
		hardware_based = Py_False;
	}
#if defined(IS_INTEL)
	else if (_crc32c_intel_probe()) {
		crc_fn = _crc32c_hw_adler;
		crc32c_init_hw_adler();
		hardware_based = Py_True;
	}
#elif defined(IS_ARM) && (defined(__linux__) || defined(linux))
	else if (_crc32c_arm64_probe()) {
		crc_fn = _crc32c_hw_arm64;
		hardware_based = Py_True;
	}
#endif
	else if (sw_mode == UNSPECIFIED || sw_mode == AUTO) {
		crc_fn = _crc32c_sw_slicing_by_8;
		hardware_based = Py_False;
	}
	else if (sw_mode == NONE) {
		PyErr_SetString(PyExc_ImportError, import_error_msg);
		return NULL;
	}

	is_big_endian = (*(const char *)(&n) == 0);

	m = PyModule_Create(&moduledef);
	if (m == NULL) {
		return NULL;
	}
	Py_INCREF(hardware_based);
	if (PyModule_AddObject(m, "hardware_based", hardware_based) < 0) {
		return NULL;
	}
	if (PyModule_AddIntConstant(m, "big_endian", is_big_endian) < 0) {
		return NULL;
	}
	return m;
}
