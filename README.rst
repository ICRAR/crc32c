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
This can be overcome by setting the ``CRC32C_SW_MODE`` environment variable
to the value ``1`` before importing the module,
in which case a software implementation is offered.

Both the hardware- and software-based algorithms
are based on `Mark Adler's code <http://stackoverflow.com/questions/17645167/implementing-sse-4-2s-crc32c-in-software/17646775>`_,
with some modifications required
to make the code more portable
and fit for inclusion within this python package.

Copyright
---------

This package is copyrighted::

 ICRAR - International Centre for Radio Astronomy Research
 (c) UWA - The University of Western Australia, 2017
 Copyright by UWA (in the framework of the ICRAR)

The original crc32c algorithms,
both software and hardware,
are copyrighted by::

 Copyright (C) 2013 Mark Adler

License
-------

This package is licensed under the LGPL-2.1 license.

The original crc32c algorithm's code,
both software and hardware,
are licensed under the BSD 3-clause license.
