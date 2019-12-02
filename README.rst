crc32c
======

.. image:: https://travis-ci.com/ICRAR/crc32c.svg?branch=master
    :target: https://travis-ci.com/ICRAR/crc32c
.. image:: https://ci.appveyor.com/api/projects/status/lamys36iude1x180/branch/master?svg=true
    :target: https://ci.appveyor.com/project/rtobar/crc32c/branch/master
.. image:: https://badge.fury.io/py/crc32c.svg
    :target: https://badge.fury.io/py/crc32c

This package implements the crc32c checksum algorithm.
It automatically chooses between a hardware-based implementation
(using the CRC32C SSE 4.2 instruction of Intel CPUs),
or a software-based one when no hardware support can be found.

Because ``crc32c`` is in PyPI, you can install it with::

 pip install crc32c

Supported platforms are Linux and OSX using the gcc and clang compilers,
and Windows using the Visual Studio compiler. Other compilers in
Windows (MinGW for instance) might work.
Binary wheels are also provided in PyPI for major platforms/architectures.

Implementation details
----------------------

By default,
if your CPU doesn't support this instruction,
the package will fallback to use a software implementation
of the crc32c checksum algorithm.
This behavior can be changed by setting
the ``CRC32C_SW_MODE`` environment variable
to one of the following values:

* ``auto``: same as if unset, will eventually be discontinued.
* ``force``: use software implementation regardless of hardware support.
* ``none``: fail to import the module with an ``ImportError``
  if no hardware support is found (old 1.x default behavior).

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
