# Changelog

## Development version

* Mention explicit support for Python 3.11.

## [2.3]

* Improved macro definition logic and platform detection
  to enable building ``universal2`` binary wheels for macOS,
  alongside ``arm64`` and ``x86_64`` ones;
  added step to GitHub Actions to generate and publish them (#28).
* Mention explicit support for Python 3.10.
* Fixed minor compilation warning in ARM64 builds.

## [2.2.post0]

* Updated GitHub Actions
  to produce ARM64 Linux binary wheels
  and publish them to PyPI.
* Removed Windows python 2.7 binary wheels.
  There are two reasons:
  cibuildwheels has removed such support,
  our Windows user base is < %5,
  and our Python 2.7 user base is also < %5.

## [2.2]

* Fixed software algorithm implementation
  to work on big endian machines (#22).
  Now the algorithm works correctly,
  with numerical (uint32_t) values
  still representing the correct checksum.
* Added a new ``big_endian`` attribute
  that indicates if the currently platform
  is big endian or not.
* Fixed compilation issues in some ARM Linux boxes (#21).

## [2.1]

* Initial hardware-based implementation
  on ARMv8 machines (#11).
* Add hardware-based implementation
  on x86 32bit machines.
  The C code was already there,
  but we did not include it for this architecture.
  As a result,
  32-bit x86 wheels should run faster now.
* Deprecated the ``crc32`` function
  in favour of the new ``crc32c`` function,
  which apart from the new, better name
  has no other changes.
* Fixed minor warning.

## [2.0.1]

* Changed binary distribution generation strategy,
  moving from a local+Travis+AppVeyor setup
  to a unified script using GitHub Actions.
* Fixed generation of source distribution
  (two C source code files were missing from the tarball).
  This bug was introduced in 1.6,
  but the source code distributions found in PyPI
  were not affected
  because the environment where they were created
  predates the bug,
  and already contained references to the two missing files.

## [2.0]

* Changed package import logic (#12).
  Instead of failing to import
  if hardware support is not found,
  the package now automatically falls back
  to using the software implementation,
  and thus can always be imported.
  Old behavior can still be obtained
  by setting ``CRC32C_SW_MODE`` to ``none``.
* Fixed cross-compilation support (#10).

## [1.7]

* Fixes wrong uploads to PyPI (#8).

## [1.6]

* Added new ``auto`` software mode to allow automatic selection
  of hardware or software backend (#5).
* Fixed compilation in non-Intel platforms (#6).
* Added support to compile with ``icc``.

## [1.5]

* Adding software implementation of ``crc32c`` algorithm,
  original developed by Mark Adler (#4).
  Software implementation is not accessible by default
  in order to maintain current importing logic,
  and is switched on by setting ``CRC32C_SW_MODE`` to ``1``.
* Changed Intel's hardware-based implementation of ``crc32c``
  from our own home-brewed code to Mark Adler's.

## [1.4]

* Added support to compile in Windows using MSVC.
* Removing experimental read/crc/write C method.
* First version with binary wheels uploaded to PyPI (#2).

## [1.3]

* Added support to compile in Windows using the MinGW compiler (#1).

## [1.2]

* First version of ``crc32c`` doing pre- and post-masking
  against ``0xffffffff``.
  This is thus the first version of the package
  that is easily usable.

## [1.1]

* Correct license reference.

## [1.0]

* Initial release.

[1.0]: https://github.com/ICRAR/crc32c/releases/tag/v1.0
[1.1]: https://github.com/ICRAR/crc32c/releases/tag/v1.1
[1.2]: https://github.com/ICRAR/crc32c/releases/tag/v1.2
[1.3]: https://github.com/ICRAR/crc32c/releases/tag/v1.3
[1.4]: https://github.com/ICRAR/crc32c/releases/tag/v1.4
[1.5]: https://github.com/ICRAR/crc32c/releases/tag/v1.5
[1.6]: https://github.com/ICRAR/crc32c/releases/tag/v1.6
[1.7]: https://github.com/ICRAR/crc32c/releases/tag/v1.7
[2.0]: https://github.com/ICRAR/crc32c/releases/tag/v2.0
[2.0.1]: https://github.com/ICRAR/crc32c/releases/tag/v2.0.1
[2.1]: https://github.com/ICRAR/crc32c/releases/tag/v2.1
[2.2]: https://github.com/ICRAR/crc32c/releases/tag/v2.2
[2.2.post0]: https://github.com/ICRAR/crc32c/releases/tag/v2.2.post0
[2.3]: https://github.com/ICRAR/crc32c/releases/tag/v2.3
