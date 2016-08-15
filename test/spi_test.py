#!/usr/bin/env python3

from time import sleep
from libsoc import gpio
from libsoc import spi

# This test is intended to be run with sliding rheostat module
# connecting to ADC1 port on Linker Base Mezzanine Card on dragonboard410c.


def main():
    spi.SPI.set_debug(1)
    gpio_cs = gpio.GPIO(18, gpio.DIRECTION_OUTPUT)
    with gpio.request_gpios([gpio_cs]):
        tx = b'\x01\x80\x00'
        with spi.SPI(0, 0, spi.MODE_0, 10000, spi.BITS_8) as spi_dev:
            while True:
                gpio_cs.set_high()
                sleep(0.00001)
                gpio_cs.set_low()
                rx = spi_dev.rw(3, tx)
                gpio_cs.set_high()

                adc_value = (rx[1] << 8) & 0b1100000000
                adc_value = adc_value | (rx[2] & 0xff)

                print("adc_value: %d" % adc_value)
                sleep(1)

if __name__ == '__main__':
    main()
