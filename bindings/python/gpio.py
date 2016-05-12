import atexit
import contextlib
import threading
import time

from ._libsoc import (
    DIRECTION_INPUT, DIRECTION_OUTPUT,
    EDGE_BOTH, EDGE_FALLING, EDGE_NONE, EDGE_RISING,
    LS_SHARED, LS_GREEDY, LS_WEAK, api
)


class InterruptHandler(threading.Thread):
    def __init__(self, gpio, interrupt_callback):
        super(InterruptHandler, self).__init__()
        self.gpio = gpio
        self.isr_cb = interrupt_callback
        self.running = False

    def run(self):
        self.running = True
        while self.running:
            if self.gpio.poll(1000):
                self.isr_cb()

    def stop(self):
        self.running = False


class GPIO(object):
    _board_config = None

    def __init__(self, id, direction, edge=EDGE_NONE, mode=LS_SHARED):
        self.id = id
        if not isinstance(id, int):
            raise TypeError('Invalid gpio id must be an "int"')
        if mode not in (LS_SHARED, LS_GREEDY, LS_WEAK):
            raise ValueError('Invalid GPIO mode: %d' % mode)
        self._validate_direction(direction, edge)
        self.mode = mode
        self._gpio = None

    def _validate_direction(self, direction, edge=EDGE_NONE):
        if direction not in (DIRECTION_INPUT, DIRECTION_OUTPUT):
            raise ValueError('Invalid GPIO direction: %d' % direction)
        edges = (EDGE_RISING, EDGE_FALLING, EDGE_NONE, EDGE_BOTH)
        if direction == DIRECTION_INPUT and edge not in edges:
            raise ValueError('Invalid GPIO edge: %d' % edge)
        self.direction = direction
        self.edge = edge

    def open(self):
        '''Opens a file descriptor to the GPIO and configures it.'''
        assert self._gpio is None
        self._gpio = api.libsoc_gpio_request(self.id, self.mode)
        if self._gpio == 0:  # NULL from native code
            raise IOError('Unable to open GPIO_%d' % self.id)
        self.set_direction(self.direction, self.edge)

    def close(self):
        '''Cleans up the memory and resources allocated by the open method.'''
        if self._gpio:
            api.libsoc_gpio_free(self._gpio)
            self._gpio = None

    def set_direction(self, direction, edge):
        self._validate_direction(direction, edge)
        api.libsoc_gpio_set_direction(self._gpio, self.direction)
        if self.direction == DIRECTION_INPUT:
            if api.libsoc_gpio_set_edge(self._gpio, self.edge) != 0:
                raise IOError('Error setting edge for GPIO_%d' % self.id)

    def set_edge(self, edge):
        self._validate_direction(self.direction, edge)
        if api.libsoc_gpio_set_edge(self._gpio, self.edge) != 0:
            raise IOError('Error setting edge for GPIO_%d' % self.id)

    def get_direction(self):
        d = api.libsoc_gpio_get_direction(self._gpio)
        if d == -1:
            raise IOError('Error reading GPIO_%d direction: %d' % self.id)
        return d

    @staticmethod
    def gpio_id(pin):
        '''Given a pin number on the board, return the actual GPIO ID.'''
        if not GPIO._board_config:
            GPIO._board_config = api.libsoc_board_init()
            atexit.register(api.libsoc_board_free, GPIO._board_config)
        gpio = api.libsoc_board_gpio_id(GPIO._board_config, pin.encode())
        if gpio == -1:
            raise ValueError('Invalid GPIO pin name(%s)' % pin)
        return gpio

    @staticmethod
    def set_debug(enabled):
        v = 0
        if enabled:
            v = 1
        api.libsoc_set_debug(v)

    def set_high(self):
        assert self.direction == DIRECTION_OUTPUT
        api.libsoc_gpio_set_level(self._gpio, 1)

    def set_low(self):
        assert self.direction == DIRECTION_OUTPUT
        api.libsoc_gpio_set_level(self._gpio, 0)

    def is_high(self):
        l = api.libsoc_gpio_get_level(self._gpio)
        if l == -1:
            raise IOError('Error reading GPIO_%d level' % self.id)
        return l == 1

    def wait_for_interrupt(self, timeout):
        assert self.direction == DIRECTION_INPUT
        if api.libsoc_gpio_wait_interrupt(self._gpio, timeout) != 0:
            raise IOError('Error waiting for interrupt on GPIO_%d' % self.id)

    def get_edge(self):
        '''Return the edge the GPIO is configured with.'''
        assert self.direction == DIRECTION_INPUT
        e = api.libsoc_gpio_get_edge(self._gpio)
        if e == -1:
            raise IOError('Error reading GPIO_%d edge' % self.id)
        return e

    def poll(self, timeout_ms=-1):
        '''Nearly the same as wait_for_interrupt, but with less overhead.
        wait_for_interrupt does some sanity checks of the GPIO settings before
        polling. Returns True if an interrupt occurred, False on an error or
        timeout
        '''
        return api.libsoc_gpio_poll(self._gpio, timeout_ms) == 0

    def start_interrupt_handler(self, interrupt_callback):
        '''Returns a thread that continuosly polls the GPIO. If an interrupt is
           encountered, the interrupt_callback function will be run. This
           thread can be stopped by calling interrupt_handler.stop()
        '''
        ih = InterruptHandler(self, interrupt_callback)
        ih.start()
        while not ih.running:
            time.sleep(0.01)
        return ih


@contextlib.contextmanager
def request_gpios(gpios):
    if not isinstance(gpios, tuple) and not isinstance(gpios, list):
        gpios = (gpios,)
    try:
        for g in gpios:
            g.open()
        yield
    finally:
        for g in reversed(gpios):
            g.close()
