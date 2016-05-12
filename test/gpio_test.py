#!/usr/bin/python
import threading
import time

from libsoc import gpio


def test_wait_for_interrupt(gpio_in, gpio_out):
    def signaller(gpio_out, event):
        event.wait()
        # wait 1/100th of a second and create a falling interrupt
        time.sleep(0.01)
        gpio_out.set_high()
        gpio_out.set_low()

    gpio_out.set_low()
    event = threading.Event()
    t = threading.Thread(target=signaller, args=(gpio_out, event))
    t.start()

    gpio_in.set_edge(gpio.EDGE_FALLING)
    event.set()
    # wait up to one second for the interrupt
    gpio_in.wait_for_interrupt(1000)


def test_interrupt_handler(gpio_in, gpio_out):
    class int_handler(object):
        hits = 0

        def cb(self):
            self.hits += 1

    gpio_out.set_low()
    gpio_in.set_direction(gpio.DIRECTION_INPUT, gpio.EDGE_FALLING)

    cb = int_handler()
    h = gpio_in.start_interrupt_handler(cb.cb)

    for x in range(100):
        time.sleep(0.01)
        gpio_out.set_high()
        gpio_out.set_low()
    time.sleep(0.1)
    h.stop()
    h.join()
    print('Sent 100 interrupts, received: %d' % cb.hits)


def main(gpio_input_id, gpio_output_id):
    gpio_in = gpio.GPIO(gpio_input_id, gpio.DIRECTION_INPUT)
    gpio_out = gpio.GPIO(gpio_output_id, gpio.DIRECTION_OUTPUT)

    with gpio.request_gpios((gpio_in, gpio_out)):
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
            gpio_in.set_edge(edge)
            assert edge == gpio_in.get_edge()

        test_wait_for_interrupt(gpio_in, gpio_out)
        test_interrupt_handler(gpio_in, gpio_out)


if __name__ == '__main__':
    import os
    gpio_input_id = int(os.environ.get('GPIO_IN', '7'))
    gpio_output_id = int(os.environ.get('GPIO_OUT', '115'))
    main(gpio_input_id, gpio_output_id)
