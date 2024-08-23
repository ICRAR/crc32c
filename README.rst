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

The project is using certain gcc/clang compiler extensions to support
building hardware-specific functions that might not be supported
by older compiler versions.


Usage
-----

The core function exposed by this module is ``crc32c(data, value=0, gil_release_mode=-1)``.
It computes the CRC32C checksum of ``data``
starting with an initial ``value`` checksum,
similarly to how the built-in ``binascii.crc32`` works.
It can thus be used like this:

.. code-block:: python

  print(crc32c.crc32c(b'hello world'))
  # 3381945770
  crc = crc32c.crc32c(b'hello')
  print(crc32c.crc32c(b' world', value=crc))
  # 3381945770

In older versions,
the function exposed by this module was called ``crc32``.
That name is still present but deprecated,
and will be removed in new versions of the library.

The ``gil_release_mode`` keyword argument
specifies whether a call of this library shall release or keep the Global Interpreter Lock.
It can be set to the following values:

* Negative: Only release the GIL when ``data`` >= 32KiB
* 0: Never release the GIL
* Positive: Always release the GIL

The ``gil_release_mode`` parameter
doesn't have any effect on free-threaded Python builds.

On top of the ``crc32c`` function,
a ``CRC32CHash(data=b"", gil_release_mode=-1)`` class is also offered.
It is modelled after the "hash objects" of the ``hashlib`` module
of the standard library. It also offers a ``checksum`` property:

.. code-block:: python

   crc32c_hash = crc32c.CRC32CHash()
   crc32c_hash.update(b'hello')
   crc32c_hash.update(b' world')
   print(crc32c_hash.checksum == crc32c.crc32c(b'hello world'))
   # True
   print(crc32c_hash.digest())
   # b'\xc9\x94e\xaa'
   digest_as_int = int.from_bytes(crc32c_hash.digest(), "big")
   print(digest_as_int == crc32c.crc32c(b'hello world'))
   # True

For more details see
the documentation on `hash objects <https://docs.python.org/3/library/hashlib.html#hash-objects>`_.

Additionally one can consult
the following module-level values:

 * ``hardware_based`` indicates if the algorithm in use
   is software- or hardware-based.
 * ``big_endian`` indicates whether the platform is big endian or not.

A benchmarking utility can be found
when executing the ``crc32c.benchmark`` module.
Consult its help with the ``-h`` flag for options.

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
* ``none``: issue a ``RuntimeWarning`` when importing the module,
  and a ``RuntimeError`` when executing the ``crc32c`` function,
  if no hardware support is found.
  In versions of this package up to 2.6
  an ``ImportError`` was raised when importing the module instead.
  In 1.x versions this was the default behaviour.

Setting the ``CRC32C_SKIP_HW_PROBE`` to ``1``
simulates platforms without hardware support.
This is available mostly for internal testing purposes.

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
