#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "libsoc_adc.h"
#include "libsoc_debug.h"

/**
 *
 * This adc_test is intended to be run on Samsung ARTIK 710 hardware,
 * however, it can work on any board with an ADC sysfs implementation,
 * if the lib/include/libsoc_adc.h file is modified accordingly.
 *
 * The ADC sysfs interface is still vendor-specific as of 2017-07-20.
 *
 * ADC sysfs Documentation: https://developer.artik.io/documentation/
 *   developer-guide/gpio/kernel-gpio.html
 */

int main(int argc, char *argv[])
{
  int ret = EXIT_FAILURE;
  adc *adc;
  int chip = 0, pin = 0, value;

  if (argc > 1) chip = atoi(argv[1]);
  if (argc > 2) pin = atoi(argv[2]);

  libsoc_set_debug(1);

  adc = libsoc_adc_request(chip, pin);

  if (!adc)
  {
    fprintf(stderr, "Failed to get ADC\n");
  }
  else
  {
    value = libsoc_adc_get_value(adc);
    if (value < 0)
    {
      fprintf(stderr, "Failed to read ADC value\n");
    }
    else
    {
      ret = EXIT_SUCCESS;
    }
    libsoc_adc_free(adc);
  }
  return ret;
}
