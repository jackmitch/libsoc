#!/usr/bin/python
'''
Test for ADC module
'''
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
        except IOError:
            pass
        print('Value read at ADC(%d, %d): %s' % (adc.chip, adc.pin, value))

if __name__ == '__main__':
    test_adc()

