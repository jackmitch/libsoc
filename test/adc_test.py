#!/usr/bin/python
'''
Test for ADC module
'''
from libsoc import ADC

def test_adc():
    '''
    built-in test routine
    '''
    for index in range(8):
        value = None
        try:
            adc = ADC(0, index)
            value = adc.read()
        except IOError:
            pass
        print('Value read at ADC(%d, %d): %s' % (adc.chip, adc.pin, value))

if __name__ == '__main__':
    test_adc()

