#!/usr/bin/python -OO
import sys
from ._libsoc import api, LS_PWM_SHARED, LS_PWM_GREEDY, LS_PWM_WEAK

PY3 = sys.version_info >= (3, 0)

MODE = {
    'shared': LS_PWM_SHARED,
    'greedy': LS_PWM_GREEDY,
    'weak': LS_PWM_WEAK
}

POLARITY = {
    'normal': 'n',
    'inverse': 'i',
    'n': 'normal',
    'i': 'inverse'
}

class PWM(object):
    '''
    Represents a GPIO configured for PWM (pulse width modulation).

    Example of use:

        with PWM(0, 0) as pwm:
            pwm.write(.25)
    '''
    def __init__(self, chip, pin, mode='shared', **kwargs):
        '''
        Initialize a PWM output.

        mode can be LS_PWM_SHARED (default), LS_PWM_GREEDY, or LS_PWM_WEAK.
        '''
        if not isinstance(chip, int):
            raise TypeError('Invalid chip id must be an "int"')
        if not isinstance(pin, int):
            raise TypeError('Invalid adc pin must be an "int"')
        self.chip = chip
        self.pin = pin
        self.mode = MODE[mode]
        self.period = kwargs.get('period')
        self.duty_cycle = kwargs.get('duty_cycle')
        self.polarity = kwargs.get('polarity')
        self._pwm = None

    period = property(
        lambda self: api.libsoc_pwm_get_period(self),
        lambda self, value: api.libsoc_pwm_set_period(self, value))

    duty_cycle = property(
        lambda self: api.libsoc_pwm_get_duty_cycle(self),
        lambda self, value: api.libsoc_pwm_set_duty_cycle(self, value))

    polarity = property(
        lambda self: POLARITY[api.libsoc_pwm_get_polarity(self)],
        lambda self, value: api.libsoc_pwm_set_polarity(self, POLARITY[value]))

    enabled = property(
        lambda self: api.libsoc_pwm_get_enabled(self),
        lambda self, value: api.libsoc_pwm_set_enabled(self, value))

    def __enter__(self):
        self.open()
        return self

    def __exit__(self, type, value, traceback):
        self.close()

    def open(self):
        '''
        Opens a file descriptor to the GPIO and configures it.
        '''
        assert self._pwm is None
        self._pwm = api.libsoc_pwm_request(self.chip, self.pin, self.mode)
        if self._pwm == 0:  # NULL from native code
            raise IOError(
                'Unable to open pwm chip(%d) pin(%d)' % (self.chip, self.pin))

    def close(self):
        '''Cleans up the memory and resources allocated by the open method.'''
        if self._pwm:
            api.libsoc_pwm_free(self._pwm)
            self._pwm = None

    @staticmethod
    def set_debug(enabled):
        v = 0
        if enabled:
            v = 1
        api.libsoc_set_debug(v)

    def on(self):
        self.enabled = ENABLED

    def off(self):
        self.enabled = DISABLED
