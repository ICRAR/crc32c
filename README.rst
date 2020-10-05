crc32c
======

.. image:: https://github.com/ICRAR/crc32c/workflows/Build%20and%20release%20to%20PyPI/badge.svg?branch=master

.. image:: https://badge.fury.io/py/crc32c.svg
    :target: https://badge.fury.io/py/crc32c

This package implements the crc32c checksum algorithm.
It automatically chooses between a hardware-based implementation
(using the CRC32C SSE 4.2 instruction of Intel CPUs,
and the crc32* instructions on ARMv8 CPUs),
or a software-based one when no hardware support can be found.

Because ``crc32c`` is in PyPI, you can install it with::

 pip install crc32c

Supported platforms are Linux and OSX using the gcc and clang compilers,
and Windows using the Visual Studio compiler. Other compilers in
Windows (MinGW for instance) might work.
Binary wheels are also provided in PyPI for major platforms/architectures.


Usage
-----

The only method exposed by this module is ``crc32c(data, [crc])``.
It computes the CRC32C checksum of ``data``
starting with an initial ``crc`` checksum,
similarly to how the built-in ``binascii.crc32`` works.
It can thus be used like this:

.. code-block:: python

  print(crc32c.crc32c(b'hello world'))
  # 3381945770
  crc = crc32c.crc32c(b'hello')
  print(crc32c.crc32c(b' world', crc))
  # 3381945770

In older versions,
the function exposed by this module was called ``crc32``.
That name is still present but deprecated,
and will be removed in new versions of the library.

Additionally one can consult
the ``hardware_based`` module-level value
to check if the algorithm currently in use
is software- or hardware-based.


Implementation details
----------------------

By default,
if your CPU doesn't have CRC32C hardware support,
the package will fallback to use a software implementation
of the crc32c checksum algorithm.
This behavior can be changed by setting
the ``CRC32C_SW_MODE`` environment variable
to one of the following values:

* ``auto``: same as if unset, will eventually be discontinued.
* ``force``: use software implementation regardless of hardware support.
* ``none``: fail to import the module with an ``ImportError``
  if no hardware support is found (old 1.x default behavior).

The software algorithm is based
on Intel's `slice-by-8 package <https://sourceforge.net/projects/slicing-by-8/>`_,
with some adaptations done
by `Evan Jones <https://www.evanjones.ca/crc32c.html>`_
and packaging provided by `Ferry Toth <https://github.com/htot/crc32c>`_.
Further adaptations were required
to make the code more portable
and fit for inclusion within this python package.

The Intel SSE 4.2 algorithm
is based on `Mark Adler's code <http://stackoverflow.com/questions/17645167/implementing-sse-4-2s-crc32c-in-software/17646775>`_,
with some modifications required
to make the code more portable
and fit for inclusion within this python package.

The ARMv8 hardware implementation
is based on Google's `crc32c <https://github.com/google/crc32c>`_
C++ library.

Copyright
---------

This package is copyrighted::

 ICRAR - International Centre for Radio Astronomy Research
 (c) UWA - The University of Western Australia, 2017
 Copyright by UWA (in the framework of the ICRAR)

The original slice-by-8 software algorithm
is copyrighted by::

 Copyright (c) 2004-2006 Intel Corporation - All Rights Reserved

Further adaptations to the slice-by-8 algorithm
previous to the inclusion in this package
are copyrighted by::

 Copyright 2008,2009,2010 Massachusetts Institute of Technology.

The original Intel SSE 4.2 crc32c algorithm
is copyrighted by::

 Copyright (C) 2013 Mark Adler

The crc32c ARMv8 hardware code
is copyrighted by::

 Copyright 2017 The CRC32C Authors

A copy of the `AUTHORS <AUTHORS.google-crc32c>`_ file
from Google's crc32c project
as it was at the time of copying the code
is included in this repository.

License
-------

This package is licensed under `the LGPL-2.1 license <LICENSE>`_.

The original slice-by-8 software algorithm
is licensed under `the 2-clause BSD licence
<https://opensource.org/licenses/bsd-license.html>`_.

Further modifications to the slice-by-8 software algorithm
are licensed under `a 3-clause BSD licence <LICENSE.slice-by-8>`_

The original Intel SSE 4.2 crc32c algorithm's code
is licensed under a custom license
embedded in the ``crc32c_adler.c`` file.

The original crc32c ARMv8 hardware code
is licensed under `a 3-clause BSD license <LICENSE.google-crc32c>`_.
