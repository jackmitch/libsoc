#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "libsoc_gpio.h"

/**
 * 
 * This gpio_test is intended to be run on beaglebone white hardware
 * and uses pins P9_42(gpio7) and P9_27 (gpio115) connected together.
 *
 * The GPIO_OUTPUT and INPUT defines can be changed to support any board
 * all you need to do is ensure that the two pins are connected together
 * 
 */
 
 #define GPIO_OUTPUT  115
 #define GPIO_INPUT   7

int main(void)
{
  // Create new gpio pointer
  gpio *gpio_output, *gpio_input;

  // Enable debug output
  libsoc_gpio_set_debug(1);

  // Request GPIO 7
  gpio_output = libsoc_gpio_request(GPIO_OUTPUT);
  gpio_input = libsoc_gpio_request(GPIO_INPUT);

  // Ensure gpio was successfully requested
  if (gpio_output == NULL || gpio_input == NULL)
  {
    goto fail;
  }
  
  // Set direction to OUTPUT
  libsoc_gpio_set_direction(gpio_output, OUTPUT);
  
  // Check the direction
  if (libsoc_gpio_get_direction(gpio_output) != OUTPUT)
  {
    printf("Failed to set direction to OUTPUT\n");
    goto fail;
  }
  
  libsoc_gpio_set_direction(gpio_input, INPUT);
  
  // Check the direction
  if (libsoc_gpio_get_direction(gpio_input) != INPUT)
  {
    printf("Failed to set direction to INPUT\n");
    goto fail;
  }
  
  int i;
  
  // Set level HIGH then LOW and check inbetween each call
  libsoc_gpio_set_level(gpio_output, HIGH);
  
  if (libsoc_gpio_get_level(gpio_output) != HIGH)
  {
    printf("Failed setting gpio level HIGH\n");
    goto fail;
  }
  
  if (libsoc_gpio_get_level(gpio_input) != HIGH)
  {
    printf("GPIO hardware read was not HIGH\n");
    goto fail;
  }
  
  libsoc_gpio_set_level(gpio_output, LOW);
  
  if (libsoc_gpio_get_level(gpio_output) != LOW)
  {
    printf("Failed setting gpio level LOW\n");
    goto fail;
  }
  
  if (libsoc_gpio_get_level(gpio_input) != LOW)
  {
    printf("GPIO hardware read was not LOW\n");
    goto fail;
  }
  
  libsoc_gpio_set_debug(0);
  
  // Toggle the GPIO 1000 times as fast as it can go
  for (i=0; i<1000; i++)
  {
    libsoc_gpio_set_level(gpio_output, HIGH);
    libsoc_gpio_set_level(gpio_output, LOW);
  }
  
  libsoc_gpio_set_debug(1);
  
  // Change edge
  libsoc_gpio_set_edge(gpio_input, RISING);
  
  // Check Edge
  if (libsoc_gpio_get_edge(gpio_input) != RISING)
  {
    printf("Failed to set edge to RISING\n");
    goto fail;
  }
  
  // Change edge
  libsoc_gpio_set_edge(gpio_input, FALLING);
  
  // Check Edge
  if (libsoc_gpio_get_edge(gpio_input) != FALLING)
  {
    printf("Failed to set edge to FALLING\n");
    goto fail;
  }
  
  // Change edge
  libsoc_gpio_set_edge(gpio_input, NONE);
  
  // Check Edge
  if (libsoc_gpio_get_edge(gpio_input) != NONE)
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
      
      // Create a falling interrupt
      libsoc_gpio_set_level(gpio_output, HIGH);
      libsoc_gpio_set_level(gpio_output, LOW);
      
      exit(EXIT_SUCCESS);
    }
  }
  else
  {
    printf("Fork failed, interrupt won't be triggered\n");
  }
  
  // Set the edge to falling in order to test interrupts
  libsoc_gpio_set_edge(gpio_input, FALLING);
  
  // Wait 10 seconds for falling interrupt to occur on gpio7
  int ret = libsoc_gpio_wait_interrupt(gpio_input, 10000);
  
  if (ret == EXIT_SUCCESS)
  {
    printf("Interrupt caught!\n");
  }
  else
  {
    printf("No interrupt triggered!\n"); 
  }
  
  int status;
  
  wait(&status);
  
  fail:
  
  // If gpio_request was successful
  if (gpio_input)
  {
    // Free gpio request memory
    libsoc_gpio_free(gpio_input);
  }
  
  if (gpio_output)
  {
    // Free gpio request memory
    libsoc_gpio_free(gpio_output);
  }
  
  return EXIT_SUCCESS;
}
