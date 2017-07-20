#!/usr/bin/python
import sys
from ctypes import create_string_buffer

from ._libsoc import api

PY3 = sys.version_info >= (3, 0)

class ADC(object):
    '''
    Represents an ADC pin.

    Example of use:

        with ADC(0, 0) as adc:
            print adc.read()
    '''
    def __init__(self, chip, pin):
        if not isinstance(chip, int):
            raise TypeError('Invalid chip id must be an "int"')
        if not isinstance(pin, int):
            raise TypeError('Invalid adc pin must be an "int"')
        self.chip = chip
        self.pin = pin
        self._adc = None

    def __enter__(self):
        self.open()
        return self

    def __exit__(self, type, value, traceback):
        self.close()

    def open(self):
        '''
        Opens a file descriptor to the GPIO and configures it.
        '''
        assert self._adc is None
        self._adc = api.libsoc_adc_request(self.chip, self.pin)
        if self._adc == 0:  # NULL from native code
            raise IOError(
                'Unable to open adc chip(%d) pin(%d)' % (self.chip, self.pin))

    def close(self):
        '''Cleans up the memory and resources allocated by the open method.'''
        if self._adc:
            api.libsoc_adc_free(self._adc)
            self._adc = None

    @staticmethod
    def set_debug(enabled):
        v = 0
        if enabled:
            v = 1
        api.libsoc_set_debug(v)

    def read(self):
        if api.libsoc_adc_get_value(self._adc) == -1:
            raise IOError('Error reading adc value')
