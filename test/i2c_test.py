#!/usr/bin/python

from libsoc import I2C

# This test is intended to be run with the Grove-LCD on I2C bus 0.
# The test initializes the LCD and then sets the background to solid blue.
LCD_BUS = 0
LCD_ADDR = 0x62


def main():
    I2C.set_debug(1)
    with I2C(LCD_BUS, LCD_ADDR) as lcd:
        # initialize LCD
        lcd.write((0x0, 0x00))
        lcd.write((0x1, 0x00))
        lcd.write((0x8, 0xAA))

        lcd.write((0x2, 0xFF))  # Make blue background
        assert b'\xFF' == lcd.read(1), 'Unable to read blue level'


if __name__ == '__main__':
    main()
