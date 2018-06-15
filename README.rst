crc32c
======

This package exposes to Python the CRC32C algorithm implemented in the SSE 4.2
instruction set of Intel CPUs.

Because ``crc32c`` is in PyPI, you can install it with::

 pip install crc32c

Supported platforms are Linux and OSX using the gcc and clang compilers.
Compilation using the Visual Studio compiler fails, but other compilers in
Windows (MinGW for instance) might work.

If your CPU doesn't support this instruction, the package will fail to load
with an ``ImportError``.
