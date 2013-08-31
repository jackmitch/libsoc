#include <stdio.h>
#include <stdlib.h>

#include "libsoc_gpio.h"

int main(void)
{
  // Create new gpio pointer
  gpio* test_gpio;

  // Enable debug output
  libsoc_gpio_set_debug(1);

  // Request GPIO 7
  test_gpio = libsoc_gpio_request(7);


  // Ensure gpio was successfully requested
  if (test_gpio == NULL)
  {
    goto fail;
  }
  
  // Set direction to OUTPUT
  libsoc_gpio_set_direction(test_gpio, OUTPUT);
  
  
  // Check the direction
  libsoc_gpio_get_direction(test_gpio);
  
  int i;
  
  // Set level HIGH then LOw and check inbetween each call
  libsoc_gpio_set_level(test_gpio, HIGH);
  libsoc_gpio_get_level(test_gpio);
  libsoc_gpio_set_level(test_gpio, LOW);
  libsoc_gpio_get_level(test_gpio);
  
  // Toggle the GPIO 1000 times as fast as it can go
  for (i=0; i<1000; i++)
  {
    libsoc_gpio_set_level(test_gpio, HIGH);
    libsoc_gpio_set_level(test_gpio, LOW);
  }
  
  // Set direction to INPUT
  libsoc_gpio_set_direction(test_gpio, INPUT);
  
  // Ensure direction set to INPUT
  libsoc_gpio_get_direction(test_gpio);
  
  // Check GPIO input level
  libsoc_gpio_get_level(test_gpio);
  
  fail:
  
  // If gpio_request was successful
  if (test_gpio)
  {
    // Free gpio request memory
    libsoc_gpio_free(test_gpio);
  }
  
  return EXIT_SUCCESS;
}
