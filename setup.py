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

import distutils.ccompiler
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext


crcmod_ext = Extension('crc32c',
                       depends=glob.glob('*.h'),
                       language='c',
                       sources=['_crc32c.c', 'crc32c_adler.c', 'crc32c_sw.c', 'checksse42.c'],
                       include_dirs=['.'])

def get_extra_compile_args():
    # msvc is treated specially; otherwise we assume it's a unix compiler
    comp = distutils.ccompiler.get_default_compiler()
    if comp == 'msvc':
        return ['/O2', '/DNDEBUG']
    else:
        return ['-O3', '-msse4.2', '-mpclmul', '-DNDEBUG']

class _build_ext(build_ext):
    """Custom build_ext command that includes extra compilation arguments"""
    def run(self):
        assert(len(self.distribution.ext_modules) == 1)
        self.distribution.ext_modules[0].extra_compile_args = get_extra_compile_args()
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
]

with open('README.rst', 'rt') as f:
    long_description = f.read()

setup(name='crc32c',
      author='The ICRAR DIA Team',
      url='https://github.com/ICRAR/crc32c',
      author_email='rtobar@icrar.org',
      version='1.5',
      license="LGPLv2.1+",
      description='A python package exposing the Intel SSE4.2 CRC32C instruction',
      long_description=long_description,
      long_description_content_type='text/x-rst',
      classifiers=classifiers,
      ext_modules=[crcmod_ext],
      cmdclass = {'build_ext': _build_ext},
      test_suite="test")
