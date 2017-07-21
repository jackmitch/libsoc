#!/usr/bin/python
'''
Simple test of the PWM API

Should duplicate function of pwm_test.c (q.v.)
'''
import sys, os, logging
from ast import literal_eval
from libsoc import PWM
PWM.set_debug(__debug__)
logging.basicConfig(level=logging.DEBUG if __debug__ else logging.INFO)

def test_pwm(chip=0, pin=1, period=10, duty_cycle=5, 
             mode='shared', polarity='normal', enabled=False):
    '''
    Duplicate pwm_test.c
    '''
    kwargs = {k: safe_int(v) for k, v in locals().items()}
    logging.debug('kwargs: %s', kwargs)
    with PWM(**kwargs) as pwm:
        for key in kwargs:
            value = getattr(pwm, key)
            logging.debug('Check value %s for pwm.%s', value, key)
        pwm.duty_cycle = pwm.period + pwm.duty_cycle
        if pwm.duty_cycle != pwm.duty_cycle:
            logging.error('PWM allowed a duty cycle > 100%')
        else:
            logging.debug('PWM properly rejected >100% duty cycle')
            pass
        pwm.on()
        if not pwm.enabled:
            logging.error('The PWM would not turn on')
        pwm.off()
        if pwm.enabled:
            logging.error('The PWM would not shut off')

def safe_int(string):
    '''
    Returns an int if it parses as one, otherwise a string
    '''
    try:
        number = literal_eval(string)
        if isinstance(number, int):
            return number
        else:
            raise TypeError('Not a number')
    except ValueError, TypeError:
        return string or None

if __name__ == '__main__':
    '''
    You can set chip and pin number on the command line
    
    Also other properties in this order:
        period: an integer up to hardware limit (1000000000 for ARTIK7)
        duty_cycle: an integer up to `period`
        mode: 'shared', 'greedy', or 'weak'
        polarity: 'normal' or 'inverse'
        enabled: 1 (on) or 0 (off)
    '''
    test_pwm(*sys.argv[1:])
