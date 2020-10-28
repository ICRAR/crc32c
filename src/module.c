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

#include "checksse42.h"
#include "common.h"
#include "crc32c.h"

/* Set at module loading time */
static crc_function crc_fn;
int is_big_endian;

static
PyObject* crc32c_crc32c(PyObject *self, PyObject *args) {
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

static
PyObject *crc32c_crc32(PyObject *self, PyObject *args)
{
	if (PyErr_WarnEx(PyExc_DeprecationWarning,
	                 "crc32c.crc32 will be eventually removed, use crc32c.crc32c instead",
	                 1) == -1) {
		return NULL;
	}
	return crc32c_crc32c(self, args);
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
	{"crc32",   crc32c_crc32,   METH_VARARGS, "Calculate crc32c incrementally (deprecated)"},
	{"crc32c",  crc32c_crc32c,  METH_VARARGS, "Calculate crc32c incrementally"},
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

/* Support for Python 2/3 */
#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef moduledef = {PyModuleDef_HEAD_INIT, "crc32c", "crc32c implementation in hardware and software", -1, CRC32CMethods};
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
		return MOD_VAL(NULL);
	}

	is_big_endian = (*(const char *)(&n) == 0);

	MOD_DEF(m, "crc32c", "crc32c implementation in hardware and software", CRC32CMethods);
	if (m == NULL) {
		return MOD_VAL(NULL);
	}
	Py_INCREF(hardware_based);
	if (PyModule_AddObject(m, "hardware_based", hardware_based) < 0) {
		return MOD_VAL(NULL);
	}
	if (PyModule_AddIntConstant(m, "big_endian", is_big_endian) < 0) {
		return MOD_VAL(NULL);
	}
	return MOD_VAL(m);
}
