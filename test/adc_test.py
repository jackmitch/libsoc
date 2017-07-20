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
        adc = ADC(0, index)
        print('Value read at ADC(%d, %d): %d' % (adc.chip, adc.pin, adc.read()))

if __name__ == '__main__':
    test_adc()

