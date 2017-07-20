#!/usr/bin/python
'''
Simple test of the PWM API

Should duplicate function of pwm_test.c (q.v.)
'''
import sys, os
from ast import literal_eval
from libsoc import PWM
PWM.set_debug(__debug__)

def test_pwm(chip=0, pin=1, mode='shared', polarity='normal', period=10,
             duty_cycle=5, enabled=False):
    '''
    Duplicate pwm_test.c
    '''
    kwargs = {k: safe_int(v) for k, v in locals().items()}
    pwm = PWM(**kwargs)
    for key in kwargs:
        value = getattr(pwm, key)
        if value != kwargs[key]:
            sys.stderr.write('Wrong value %s for pwm.%s\n' % (value, key))
    try:
        pwm.duty_cycle = pwm.period + 5
        raise SystemError('The system allowed a duty cycle over 100%')
    except ValueError:
        pass
    pwm.on()
    if not pwm.enabled:
        raise SystemError('The PWM would not turn on')
    pwm.off()
    if pwm.enabled:
        raise SystemError('The PWM would not shut off')

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
        return string

if __name__ == '__main__':
    '''
    You can set chip and pin number on the command line
    
    Also other properties in this order:
        mode: 'shared', 'greedy', or 'weak'
        polarity: 'normal' or 'inverse'
        period: an integer up to hardware limit (1000000000 for ARTIK7)
        duty_cycle: an integer up to `period`
        enabled: 1 (on) or 0 (off)
    '''
    test_pwm(*sys.argv[1:])
