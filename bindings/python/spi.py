import sys

from ctypes import create_string_buffer

from ._libsoc import (
    BITS_8, BITS_16, BPW_ERROR,
    MODE_0, MODE_1, MODE_2, MODE_3, MODE_ERROR, api
)

PY3 = sys.version_info >= (3, 0)


class SPI(object):
    def __init__(self, spidev_device, chip_select, mode, speed, bpw):
        if not isinstance(spidev_device, int):
            raise TypeError('Invalid spi device id must be an "int"')
        if not isinstance(chip_select, int):
            raise TypeError('Invalid spi chip select must be an "int"')
        if mode not in (MODE_0, MODE_1, MODE_2, MODE_3):
            raise ValueError('Invalid mode: %d' % mode)
        if not isinstance(speed, int):
            raise TypeError('Invalid speed must be an "int"')
        if bpw not in (BITS_8, BITS_16):
            raise ValueError('Invalid bits per word: %d' % bpw)
        self.device = spidev_device
        self.chip = chip_select
        self.mode = mode
        self.speed = speed
        self.bpw = bpw
        self._spi = None

    def __enter__(self):
        self.open()
        return self

    def __exit__(self, type, value, traceback):
        self.close()

    def open(self):
        assert self._spi is None
        self._spi = api.libsoc_spi_init(self.device, self.chip)
        if self._spi == 0:
            raise IOError('Unable to open spi device(%d)' % self.device)
        self.set_mode(self.mode)
        if self.get_mode() != self.mode:
            raise IOError('Set mode incorrectly')
        self.set_speed(self.speed)
        if self.get_speed() != self.speed:
            raise IOError('Set speed incorrectly')
        self.set_bits_per_word(self.bpw)
        if self.get_bits_per_word() != self.bpw:
            raise IOError('Set bits per word incorrectly')

    def close(self):
        if self._spi:
            api.libsoc_spi_free(self._spi)
            self._spi = None

    @staticmethod
    def set_debug(enabled):
        v = 0
        if enabled:
            v = 1
        api.libsoc_set_debug(v)

    def set_bits_per_word(self, bpw):
        if bpw not in (BITS_8, BITS_16):
            raise ValueError('Invalid bits per word: %d' % bpw)
        self.bpw = bpw
        api.libsoc_spi_set_bits_per_word(self._spi, self.bpw)

    def get_bits_per_word(self):
        b = api.libsoc_spi_get_bits_per_word(self._spi)
        if b == BPW_ERROR:
            raise IOError('bits per word not recognized')
        return b

    def set_mode(self, mode):
        assert self._spi is not None
        if mode not in (MODE_0, MODE_1, MODE_2, MODE_3):
            raise ValueError('Invalid mode: %d' % mode)
        self.mode = mode
        api.libsoc_spi_set_mode(self._spi, self.mode)

    def get_mode(self):
        m = api.libsoc_spi_get_mode(self._spi)
        if m == MODE_ERROR:
            raise IOError('mode not recognized')
        return m

    def set_speed(self, speed):
        if not isinstance(speed, int):
            raise TypeError('Invalid speed must be an "int"')
        self.speed = speed
        api.libsoc_spi_set_speed(self._spi, self.speed)

    def get_speed(self):
        s = api.libsoc_spi_get_speed(self._spi)
        if s == -1:
            raise IOError('failed reading speed')
        return s

    def read(self, num_bytes):
        assert num_bytes > 0
        buff = create_string_buffer(num_bytes)
        if api.libsoc_spi_read(self._spi, buff, num_bytes) == -1:
            raise IOError('Error reading spi device')
        return buff.raw

    def write(self, byte_array):
        assert len(byte_array) > 0
        if PY3:
            buff = bytes(byte_array)
        else:
            buff = ''.join(map(chr, byte_array))
        api.libsoc_spi_write(self._spi, buff, len(buff))

    def rw(self, num_bytes, byte_array):
        assert num_bytes > 0
        assert len(byte_array) > 0
        rbuff = create_string_buffer(num_bytes)
        if PY3:
            wbuff = bytes(byte_array)
        else:
            wbuff = ''.join(map(chr, byte_array))
        if api.libsoc_spi_rw(self._spi, wbuff, rbuff, num_bytes) != 0:
            raise IOError('Error rw spi device')
        return rbuff.raw
