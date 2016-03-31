import sys
from ctypes import create_string_buffer

from ._libsoc import api

PY3 = sys.version_info >= (3, 0)


class I2C(object):
    def __init__(self, bus, address):
        if not isinstance(bus, int):
            raise TypeError('Invalid bus id must be an "int"')
        if not isinstance(address, int):
            raise TypeError('Invalid bus address must be an "int"')
        self.bus = bus
        self.addr = address
        self._i2c = None

    def __enter__(self):
        self.open()
        return self

    def __exit__(self, type, value, traceback):
        self.close()

    def open(self):
        '''Opens a file descriptor to the GPIO and configures it.'''
        assert self._i2c is None
        self._i2c = api.libsoc_i2c_init(self.bus, self.addr)
        if self._i2c == 0:  # NULL from native code
            raise IOError(
                'Unable to open i2c bus(%d) addr(%d)' % (self.bus, self.addr))

    def close(self):
        '''Cleans up the memory and resources allocated by the open method.'''
        if self._i2c:
            api.libsoc_i2c_free(self._i2c)
            self._i2c = None

    @staticmethod
    def set_debug(enabled):
        v = 0
        if enabled:
            v = 1
        api.libsoc_set_debug(v)

    def set_timeout(self, timeout):
        if not isinstance(timeout, int):
            raise TypeError('Invalid timeout must be an "int"')
        api.libsoc_i2c_set_timeout(self._i2c, timeout)

    def read(self, num_bytes):
        buff = create_string_buffer(num_bytes)
        if api.libsoc_i2c_read(self._i2c, buff, num_bytes) == -1:
            raise IOError('Error reading i2c device')
        return buff.value

    def write(self, byte_array):
        if PY3:
            buff = bytes(byte_array)
        else:
            buff = ''.join(map(chr, byte_array))
        api.libsoc_i2c_write(self._i2c, buff, len(buff))
