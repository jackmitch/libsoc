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

MODE.update({v: k for k, v in MODE.items()})

POLARITY = {
    'normal': 0,
    'inversed': 1,
    'error': -1,
}

POLARITY.update({v: k for k, v in POLARITY.items()})

class PWM(object):
    '''
    Represents a GPIO configured for PWM (pulse width modulation).

    Example of use:

        with PWM(0, 0) as pwm:
            pwm.enable = 1
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
        self.mode = mode
        self.kwargs = kwargs
        logging.debug('self.kwargs: %s', self.kwargs)
        self._pwm = None

    period = property(
        lambda self: api.libsoc_pwm_get_period(self._pwm),
        lambda self, value: api.libsoc_pwm_set_period(self._pwm, value))

    duty_cycle = property(
        lambda self: api.libsoc_pwm_get_duty_cycle(self._pwm),
        lambda self, value: api.libsoc_pwm_set_duty_cycle(self._pwm, value))

    polarity = property(
        lambda self: POLARITY[api.libsoc_pwm_get_polarity(self._pwm)],
        lambda self, value: api.libsoc_pwm_set_polarity(self._pwm,
                                                        POLARITY[value])) 

    enabled = property(
        lambda self: api.libsoc_pwm_get_enabled(self._pwm),
        lambda self, value: api.libsoc_pwm_set_enabled(self._pwm, value))

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
        logging.debug('Opening PWM for use')
        self._pwm = api.libsoc_pwm_request(self.chip, self.pin, MODE[self.mode])
        if self._pwm == 0:  # NULL from native code
            raise IOError(
                'Unable to open pwm chip(%d) pin(%d)' % (self.chip, self.pin))
        logging.debug('Setting values: %s', self.kwargs)
        if self.kwargs.get('period') is not None:
            logging.debug('setting period to %s', self.kwargs['period'])
            self.period = self.kwargs['period']
        if self.kwargs.get('duty_cycle') is not None:
            logging.debug('setting duty cycle to %s', self.kwargs['duty_cycle'])
            self.duty_cycle = self.kwargs['duty_cycle']
        if self.kwargs.get('polarity') is not None:
            logging.debug('setting polarity to %s', self.kwargs['polarity'])
            self.polarity = POLARITY[self.kwargs['polarity']]
        if self.kwargs.get('enabled') is not None:
            logging.debug('setting enabled to %s', self.kwargs['enabled'])
            self.enabled = self.kwargs['enabled']

    def close(self):
        '''
        Cleans up the memory and resources allocated by the open method.

        Note that in "shared" mode it does not unexport the pin: see
        lib/pwm.c.
        '''
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
        self.enabled = 1

    def off(self):
        self.enabled = 0
