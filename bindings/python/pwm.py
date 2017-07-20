#!/usr/bin/python -OO
import sys, logging
from ._libsoc import api, LS_PWM_SHARED, LS_PWM_GREEDY, LS_PWM_WEAK
logging.basicConfig(level=logging.DEBUG if __debug__ else logging.INFO)

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

        mode can be 'shared' (default), 'greedy', or 'weak'
        '''
        if not isinstance(chip, int):
            raise TypeError('Invalid chip id must be an "int"')
        if not isinstance(pin, int):
            raise TypeError('Invalid adc pin must be an "int"')
        self.chip = chip
        self.pin = pin
        logging.debug("locals(): %s", locals())
        self.mode = mode
        self.period = kwargs.get('period', None)
        self.duty_cycle = kwargs.get('duty_cycle', None)
        self.polarity = kwargs.get('polarity', None)
        self.enabled = kwargs.get('enabled', None)
        self._pwm = None

    pwm_period = property(
        lambda self: api.libsoc_pwm_get_period(self._pwm),
        lambda self, value: api.libsoc_pwm_set_period(self._pwm, value))

    pwm_duty_cycle = property(
        lambda self: api.libsoc_pwm_get_duty_cycle(self._pwm),
        lambda self, value: api.libsoc_pwm_set_duty_cycle(self._pwm, value))

    pwm_polarity = property(
        lambda self: POLARITY[api.libsoc_pwm_get_polarity(self._pwm)],
        lambda self, value: api.libsoc_pwm_set_polarity(self._pwm,
                                                        POLARITY[value])) 

    pwm_enabled = property(
        lambda self: api.libsoc_pwm_get_enabled(self._pwm),
        lambda self, value: api.libsoc_pwm_set_enabled(self._pwm, value))

    def __enter__(self):
        self.open()
        if self.period is not None:
            self.pwm_period = self.period
        if self.duty_cycle is not None:
            self.pwm_duty_cycle = self.duty_cycle
        if self.polarity is not None:
            self.pwm_polarity = POLARITY[self.polarity]
        if self.enabled is not None:
            self.pwm_enabled = self.enabled
        return self

    def __exit__(self, type, value, traceback):
        self.close()

    def open(self):
        '''
        Opens a file descriptor to the GPIO and configures it.
        '''
        assert self._pwm is None
        self._pwm = api.libsoc_pwm_request(self.chip, self.pin, MODE[self.mode])
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
