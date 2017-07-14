#!/usr/bin/python -OO
'''
Switched LED test, using interrupts on inputs and driving outputs.

Assumes switches are pulled low when pressed, and output must be
high to drive LED.

The default values are for the ARTIK 710 developer board, with mapping
from https://developer.artik.io/documentation/developer-guide/gpio/
gpio-mapping.html.

Device           Sysfs mapping     Note

SW403            GPIO 30           Nearest board edge, next to red LED400
SW404            GPIO 32           Next to blue LED401
LED400           GPIO 28           Red LED
LED401           GPIO 38           Blue LED
'''
import sys
import time
import logging
from libsoc import gpio
# run as `python switched_led.py` for verbose logging
logging.basicConfig(level=logging.DEBUG if __debug__ else logging.INFO)
PAIRS = {}  # switch-LED pairs
NAMES = {   # edit for your board, or set to empty dict
    30: 'SW403',
    32: 'SW404',
    28: 'LED400',
    38: 'LED401',
}

def switchtest(gpios):
    '''
    Run the switched LED test with given GPIO numbers.
    '''
    switches = gpios[::2]
    leds = gpios[1::2]
    for switch, led in zip(switches, leds):
        switch_signal = gpio.GPIO(
            switch, gpio.DIRECTION_INPUT, gpio.EDGE_BOTH)
        led_signal = gpio.GPIO(led, gpio.DIRECTION_OUTPUT)
        with led_signal as signal:
            signal.set_low()  # start with all LEDs off
        PAIRS[switch_signal] = led_signal
    # now the pairs are established, we can enable interrupts
    logging.debug('PAIRS: %s', PAIRS)
    try:
        with gpio.request_gpios(PAIRS.keys() + PAIRS.values()):
            for switch_signal in PAIRS:
                switch_signal.start_interrupt_handler(
                    activator, args=(switch_signal,))
            logging.info('Push a switch to see its LED light; '
                         'Control-C to exit.')
            while True:
                time.sleep(60)  # wait until user gets bored
    except KeyboardInterrupt:
        sys.exit

def activator(signal):
    '''
    Called when a switch transitions to high or low.

    We turn LED on or off depending on whether the switch is down or up.
    '''
    logging.debug('Switch %s activated', NAMES.get(signal.id, signal.id))
    if signal.is_high():
        PAIRS[signal].set_low()
    else:
        PAIRS[signal].set_high()

if __name__ == '__main__':
    gpios = map(int, sys.argv[1:]) or [30, 28, 32, 38]
    logging.debug('GPIOs: %s', gpios)
    if len(gpios) % 2 == 0:
        switchtest(gpios)
    else:
        logging.fatal('Must specify GPIOs of each switch followed by its LED')

