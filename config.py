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

try:
    import configparser
except ImportError:
    # Python 2 compatibility
    import ConfigParser as configparser

class setup_config(object):
    """
    Class to obtain configuration provided by the setup.cfg file used by
    setuptools.
    """

    # Setuptools configuration file
    SETUPTOOLS_CONFIG_FILE = 'setup.cfg'

    # Cross-compiling section
    BUILD_SECTION_NAME = 'build_info'

    # Key name for specifying the target machine
    MACHINE_KEY = 'machine'

    def __init__(self):
        """Initialize the configuration object"""
        self._metadata = self._metadata_from_setupcfg(self.BUILD_SECTION_NAME)

    def machine(self):
        """Return the target machine name, or None if unknown"""
        return self._get_cfg_value(self.MACHINE_KEY)

    def _get_cfg_value(self, key):
        """
        Get a value associated with a key in the build section. Returns the
        value, or None if unknown.
        """
        return self._metadata.get(key, None)

    @classmethod
    def _metadata_from_setupcfg(cls, section_name):
        """Read the section of setup.cfg and return it as a dict"""
        cfgparser = configparser.ConfigParser()
        cfgparser.read(cls._get_cfg_filename())

        return dict(cfgparser.items(section_name))

    @classmethod
    def _get_cfg_filename(cls):
        """Get the file name of the setuptools cfg file"""
        return cls.SETUPTOOLS_CONFIG_FILE
