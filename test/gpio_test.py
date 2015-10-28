#!/usr/bin/python
import threading
import time

from libsoc import gpio


def test_wait_for_interrupt(gpio_in, gpio_out):
    def signaller(gpio_out, event):
        event.wait()
        # wait 1/10th of a second and create a falling interrupt
        time.sleep(0.1)
        gpio_out.set_high()
        gpio_out.set_low()

    gpio_out.set_low()
    event = threading.Event()
    t = threading.Thread(target=signaller, args=(gpio_out, event))
    t.start()

    gpio_in._lib.libsoc_gpio_set_edge(gpio_in._gpio, gpio.EDGE_FALLING)
    event.set()
    # wait up to one second for the interrupt
    gpio_in.wait_for_interrupt(1000)


class gpio_cb(object):
    def __init__(self):
        self.hits = 0

    def callback(self):
        self.hits += 1


def main(gpio_in, gpio_out):
    gpio_in = gpio.GPIO(gpio_in, gpio.DIRECTION_INPUT)
    gpio_out = gpio.GPIO(gpio_out, gpio.DIRECTION_OUTPUT)

    with gpio.activate_gpios((gpio_in, gpio_out)):
        assert gpio.DIRECTION_INPUT == gpio_in.get_direction()
        assert gpio.DIRECTION_OUTPUT == gpio_out.get_direction()

        gpio_out.set_high()
        assert gpio_out.is_high()
        assert gpio_in.is_high()

        gpio_out.set_low()
        assert not gpio_out.is_high()
        assert not gpio_in.is_high()

        gpio.GPIO.set_debug(False)
        for i in range(1000):
            gpio_out.set_high()
            gpio_out.set_low()

        gpio.GPIO.set_debug(True)
        edges = (gpio.EDGE_RISING, gpio.EDGE_FALLING,
                 gpio.EDGE_BOTH, gpio.EDGE_NONE)
        for edge in edges:
            gpio_in._lib.libsoc_gpio_set_edge(gpio_in._gpio, edge)
            assert edge == gpio_in.get_edge()

        test_wait_for_interrupt(gpio_in, gpio_out)

    cb = gpio_cb()
    gpio_in = gpio.GPIO(gpio_in.id, gpio.DIRECTION_INPUT,
                        edge=gpio.EDGE_FALLING, callback=cb.callback)

    interrupts = 10
    with gpio.activate_gpios((gpio_in, gpio_out)):
        gpio.GPIO.set_debug(False)
        for i in range(interrupts):
            # create falling interrupt
            gpio_out.set_high()
            gpio_out.set_low()
            time.sleep(0.01)
        gpio.GPIO.set_debug(True)
    print('Interrupts triggered: %d, interrupts caught: %d' % (
        interrupts, cb.hits))

if __name__ == '__main__':
    import os
    gpio_in = int(os.environ.get('GPIO_IN', '7'))
    gpio_out = int(os.environ.get('GPIO_OUT', '115'))
    main(gpio_in, gpio_out)
