crc32c
======

This package exposes to Python the CRC32C algorithm implemented in the SSE 4.2
instruction set of Intel CPUs.

Because ``crc32c`` is in PyPI, you can install it with::

 pip install crc32c

If your CPU doesn't support this instruction, the package will fail to load
with an ``ImportError``.
