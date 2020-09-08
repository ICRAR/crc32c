# Changelog

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
  which appart from the new, better name
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
  because the environment were they were created
  predates the bug,
  and already contained refrences to the two missing files.

## [2.0]

* Changed package import logic (#12).
  Instead of failing to import
  if hardware support is not found,
  the package now automatically fallsback
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
