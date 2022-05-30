#
#    ICRAR - International Centre for Radio Astronomy Research
#    (c) UWA - The University of Western Australia, 2014
#    Copyright by UWA (in the framework of the ICRAR)
#    All rights reserved
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#    MA 02111-1307  USA
#
import glob
import sysconfig

import distutils.ccompiler
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext


crcmod_ext = Extension('crc32c',
                       define_macros=[('NDEBUG', None)],
                       depends=glob.glob('*.h'),
                       language='c',
                       sources=['_crc32c.c', 'checkarm.c', 'checksse42.c', 'crc32c_adler.c', 'crc32c_arm64.c', 'crc32c_sw.c'],
                       include_dirs=['.'])

def get_extra_compile_args(is_intel, is_arm):
    # msvc is treated specially; otherwise we assume it's a unix compiler
    comp = distutils.ccompiler.get_default_compiler()
    if comp == 'msvc':
        return ['/O2']
    elif is_intel:
        return ['-O3', '-msse4.2', '-mpclmul']
    elif is_arm:
        return ['-O3', '-march=armv8-a+crc+crypto']
    else:
        return ['-O3']

class _build_ext(build_ext):
    """Custom build_ext command that includes extra compilation arguments"""

    user_options = build_ext.user_options + [
        ('platform=', None, 'The target platform name')]

    def initialize_options(self):
        build_ext.initialize_options(self)
        self.platform = sysconfig.get_platform()

    def run(self):
        assert(len(self.distribution.ext_modules) == 1)
        platform = self.platform.lower()
        is_arm = any(arch in platform for arch in ('aarch64_be', 'aarch64', 'armv8b', 'armv8l', 'universal2'))
        is_intel = any(arch in platform for arch in ('x86_64', 'amd64', 'i386', 'i686', 'universal2'))
        distutils.log.info("platform: %s, is_intel: %d, is_arm: %d", platform, is_intel, is_arm)
        self.distribution.ext_modules[0].extra_compile_args = get_extra_compile_args(is_intel, is_arm)
        build_ext.run(self)


classifiers = [
    # There's no more specific classifier for LGPLv2.1+
    "License :: OSI Approved :: GNU Lesser General Public License v2 or later (LGPLv2+)",
    "Operating System :: OS Independent",
    "Programming Language :: C",
    "Programming Language :: Python :: 2.7",
    "Programming Language :: Python :: 3.2",
    "Programming Language :: Python :: 3.3",
    "Programming Language :: Python :: 3.4",
    "Programming Language :: Python :: 3.5",
    "Programming Language :: Python :: 3.6",
    "Programming Language :: Python :: 3.7",
    "Programming Language :: Python :: 3.8",
    "Programming Language :: Python :: 3.9",
    "Programming Language :: Python :: 3.10",
]

with open('README.rst', 'rt') as f:
    long_description = f.read()

setup(name='crc32c',
      author='The ICRAR DIA Team',
      url='https://github.com/ICRAR/crc32c',
      author_email='rtobar@icrar.org',
      version='2.3',
      license="LGPLv2.1+",
      description=('A python package implementing the crc32c algorithm'
      ' in hardware and software'),
      long_description=long_description,
      long_description_content_type='text/x-rst',
      classifiers=classifiers,
      ext_modules=[crcmod_ext],
      cmdclass = {'build_ext': _build_ext},
      test_suite="test")
