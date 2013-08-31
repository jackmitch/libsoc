#include <stdio.h>
#include <stdlib.h>

#include "gpio.h"

int main(void)
{
  gpio* test_gpio;

  libsoc_gpio_set_debug(1);

  test_gpio = libsoc_gpio_request(7);

  if (test_gpio == NULL)
  {
    goto fail;
  }
  
  libsoc_gpio_set_direction(test_gpio, OUTPUT);
  
  libsoc_gpio_get_direction(test_gpio);
  
  int i;
  
  libsoc_gpio_set_level(test_gpio, HIGH);
  libsoc_gpio_get_level(test_gpio);
  libsoc_gpio_set_level(test_gpio, LOW);
  libsoc_gpio_get_level(test_gpio);
  
  for (i=0; i<1000; i++)
  {
    libsoc_gpio_set_level(test_gpio, HIGH);
    libsoc_gpio_set_level(test_gpio, LOW);
  }
  
  libsoc_gpio_set_direction(test_gpio, INPUT);
  
  libsoc_gpio_get_direction(test_gpio);
  
  fail:
  
  if (test_gpio)
  {
    libsoc_gpio_free(test_gpio);
  }
  
  return EXIT_SUCCESS;
}
