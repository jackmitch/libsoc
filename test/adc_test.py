#!/usr/bin/python -OO
'''
Test for ADC module
'''
import sys, os
from libsoc import ADC
ADC.set_debug(__debug__)

def test_adc():
    '''
    ADC test routine
    '''
    for index in range(8):
        value = None
        try:
            with ADC(0, index) as adc:
                value = adc.read()
                print('Value read at ADC(%d, %d): %s' % (
                      adc.chip, adc.pin, value))
        except IOError:
            sys.stderr.write('No such ADC (%d, %d)\n' % (0, index))            

if __name__ == '__main__':
    test_adc()

