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

from setuptools import Extension, setup

crcmod_ext = Extension(
    "crc32c._crc32c",
    define_macros=[("NDEBUG", None)],
    depends=glob.glob("src/crc32c/ext/*.h"),
    language="c",
    sources=glob.glob("src/crc32c/ext/*.c"),
    include_dirs=["src/cc32c/ext/"],
)

classifiers = [
    # There's no more specific classifier for LGPLv2.1+
    "License :: OSI Approved :: GNU Lesser General Public License v2 or later (LGPLv2+)",
    "Operating System :: OS Independent",
    "Programming Language :: C",
    "Programming Language :: Python :: 3.7",
    "Programming Language :: Python :: 3.8",
    "Programming Language :: Python :: 3.9",
    "Programming Language :: Python :: 3.10",
    "Programming Language :: Python :: 3.11",
    "Programming Language :: Python :: 3.12",
    "Programming Language :: Python :: 3.13",
]

with open("README.rst", "rt") as f:
    long_description = f.read()

setup(
    name="crc32c",
    author="The ICRAR DIA Team",
    url="https://github.com/ICRAR/crc32c",
    author_email="rtobar@icrar.org",
    version="2.7.1",
    license="LGPL-2.1-or-later",
    description=(
        "A python package implementing the crc32c algorithm" " in hardware and software"
    ),
    long_description=long_description,
    long_description_content_type="text/x-rst",
    classifiers=classifiers,
    packages=["crc32c"],
    package_dir={"": "src"},
    package_data={"crc32c": ["*.pyi", "py.typed", "ext/*.h"]},
    python_requires=">=3.7",
    ext_modules=[crcmod_ext],
    test_suite="test",
)
