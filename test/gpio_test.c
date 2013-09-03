#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "libsoc_gpio.h"

/**
 * 
 * This gpio_test is intended to be run on beaglebone white hardware
 * and uses pins P9_42(gpio7) and P9_27 (gpio115) connected together.
 *
 */

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
  if (libsoc_gpio_get_direction(test_gpio) != OUTPUT)
  {
    printf("Failed to set direction to OUTPUT\n");
    goto fail;
  }
  
  int i;
  
  // Set level HIGH then LOw and check inbetween each call
  libsoc_gpio_set_level(test_gpio, HIGH);
  
  if (libsoc_gpio_get_level(test_gpio) != HIGH)
  {
    printf("Failed setting gpio level HIGH\n");
    goto fail;
  }
  
  libsoc_gpio_set_level(test_gpio, LOW);
  
  if (libsoc_gpio_get_level(test_gpio) != LOW)
  {
    printf("Failed setting gpio level LOW\n");
    goto fail;
  }
  
  libsoc_gpio_set_debug(0);
  
  // Toggle the GPIO 1000 times as fast as it can go
  for (i=0; i<1000; i++)
  {
    libsoc_gpio_set_level(test_gpio, HIGH);
    libsoc_gpio_set_level(test_gpio, LOW);
  }
  
  libsoc_gpio_set_debug(1);
  
  // Set direction to INPUT
  libsoc_gpio_set_direction(test_gpio, INPUT);
  
  // Check the direction
  if (libsoc_gpio_get_direction(test_gpio) != INPUT)
  {
    printf("Failed to set direction to INPUT\n");
    goto fail;
  }
  
  // Check GPIO input level
  libsoc_gpio_get_level(test_gpio);
  
  // Change edge
  libsoc_gpio_set_edge(test_gpio, RISING);
  
  // Check Edge
  if (libsoc_gpio_get_edge(test_gpio) != RISING)
  {
    printf("Failed to set edge to RISING\n");
    goto fail;
  }
  
  // Change edge
  libsoc_gpio_set_edge(test_gpio, FALLING);
  
  // Check Edge
  if (libsoc_gpio_get_edge(test_gpio) != FALLING)
  {
    printf("Failed to set edge to FALLING\n");
    goto fail;
  }
  
  // Change edge
  libsoc_gpio_set_edge(test_gpio, NONE);
  
  // Check Edge
  if (libsoc_gpio_get_edge(test_gpio) != NONE)
  {
    printf("Failed to set edge to NONE\n");
    goto fail;
  }
  
  pid_t childPID;

  // Fork the process so the parent process can wait for the interrupt 
  // on gpio7 and the child process can generate the interrupt from
  // gpio115
  
  childPID = fork();

  if(childPID >= 0)
  {
    if (childPID == 0)
    {
      // Allow time to setup the interrupt
      sleep(1);
      
      gpio* test_interrupt_gpio;
    
      // Request new gpio
      test_interrupt_gpio = libsoc_gpio_request(115);

      // Ensure gpio was successfully requested
      if (test_interrupt_gpio != NULL)
      {
        // Set direction to OUTPUT
        libsoc_gpio_set_direction(test_interrupt_gpio, OUTPUT);
        
        // Create a falling interrupt
        libsoc_gpio_set_level(test_interrupt_gpio, HIGH);
        libsoc_gpio_set_level(test_interrupt_gpio, LOW);
        
        // Free gpio request memory
        libsoc_gpio_free(test_interrupt_gpio);
        
        exit(EXIT_SUCCESS);
      }
      
      exit(EXIT_FAILURE);
    }
    
  }
  else
  {
    printf("Fork failed, interrupt won't be triggered\n");
  }
  
  // Set the edge to falling in order to test interrupts
  libsoc_gpio_set_edge(test_gpio, FALLING);
  
  // Wait 10 seconds for falling interrupt to occur on gpio7
  int ret = libsoc_gpio_wait_interrupt(test_gpio, 10000);
  
  if (ret == EXIT_SUCCESS)
  {
    printf("Interrupt caught!\n");
  }
  else
  {
    printf("No interrupt triggered!\n"); 
  }
  
  fail:
  
  // If gpio_request was successful
  if (test_gpio)
  {
    // Free gpio request memory
    libsoc_gpio_free(test_gpio);
  }
  
  return EXIT_SUCCESS;
}
