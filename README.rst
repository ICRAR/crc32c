crc32c
======

.. image:: https://travis-ci.org/ICRAR/crc32c.svg?branch=master
    :target: https://travis-ci.org/ICRAR/crc32c
.. image:: https://ci.appveyor.com/api/projects/status/lamys36iude1x180/branch/master?svg=true
    :target: https://ci.appveyor.com/project/rtobar/crc32c/branch/master
.. image:: https://badge.fury.io/py/crc32c.svg
    :target: https://badge.fury.io/py/crc32c

This package exposes to Python the CRC32C algorithm implemented in the SSE 4.2
instruction set of Intel CPUs.

Because ``crc32c`` is in PyPI, you can install it with::

 pip install crc32c

Supported platforms are Linux and OSX using the gcc and clang compilers,
and Windows using the Visual Studio compiler. Other compilers in
Windows (MinGW for instance) might work.

If your CPU doesn't support this instruction, the package will fail to load
with an ``ImportError``.
