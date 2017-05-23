#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "libsoc_gpio.h"
#include "libsoc_debug.h"

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

static int interrupt_count = 0;

int callback_test(void* arg)
{
  int* tmp_count = (int*) arg;
  
  *tmp_count = *tmp_count + 1;
  
  return EXIT_SUCCESS;
}


int main(void)
{
  // Create both gpio pointers
  gpio *gpio_output, *gpio_input;

  // Enable debug output
  libsoc_set_debug(1);

  // Request gpios
  gpio_output = libsoc_gpio_request(GPIO_OUTPUT, LS_GPIO_SHARED);
  gpio_input = libsoc_gpio_request(GPIO_INPUT, LS_GPIO_SHARED);

  // Ensure both gpio were successfully requested
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
  
  // Set direction to INPUT
  libsoc_gpio_set_direction(gpio_input, INPUT);
  
  // Check the direction
  if (libsoc_gpio_get_direction(gpio_input) != INPUT)
  {
    printf("Failed to set direction to INPUT\n");
    goto fail;
  }
  
  // Set level HIGH and check level in software and hardware
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
  
  // Set level LOW and check level in software and hardware
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
  
  // Turn off debug printing for fast toggle
  libsoc_set_debug(0);
  
  int i;
  
  // Toggle the GPIO 1000 times as fast as it can go
  for (i=0; i<1000; i++)
  {
    libsoc_gpio_set_level(gpio_output, HIGH);
    libsoc_gpio_set_level(gpio_output, LOW);
  }
  
  // Turn debug back on
  libsoc_set_debug(1);
  
  // Set edge to RISING
  libsoc_gpio_set_edge(gpio_input, RISING);
  
  // Check Edge
  if (libsoc_gpio_get_edge(gpio_input) != RISING)
  {
    printf("Failed to set edge to RISING\n");
    goto fail;
  }
  
  // Set edge to FALLING
  libsoc_gpio_set_edge(gpio_input, FALLING);
  
  // Check Edge
  if (libsoc_gpio_get_edge(gpio_input) != FALLING)
  {
    printf("Failed to set edge to FALLING\n");
    goto fail;
  }
  
  // Set edge to BOTH
  libsoc_gpio_set_edge(gpio_input, BOTH);
  
  // Check Edge
  if (libsoc_gpio_get_edge(gpio_input) != BOTH)
  {
    printf("Failed to set edge to BOTH\n");
    goto fail;
  }

  // Set edge to NONE
  libsoc_gpio_set_edge(gpio_input, NONE);
  
  // Check Edge
  if (libsoc_gpio_get_edge(gpio_input) != NONE)
  {
    printf("Failed to set edge to NONE\n");
    goto fail;
  }
  
  pid_t childPID;

  // Fork the process so the parent process can wait for the interrupt 
  // on GPIO_INPUT and the child process can generate the interrupt from
  // GPIO_OUTPUT
  
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
  
  // Wait 10 seconds for falling interrupt to occur on GPIO_INPUT
  int ret = libsoc_gpio_wait_interrupt(gpio_input, 10000);
  
  if (ret == LS_INT_TRIGGERED)
  {
    printf("Interrupt caught!\n");
  }
  else
  {
    printf("Interrupt missed!\n"); 
  }
  
  int status;
  
  wait(&status);
  
  
  // Setup callback
  libsoc_gpio_callback_interrupt(gpio_input, &callback_test, (void*) &interrupt_count);
  
  // Turn off debug
  libsoc_set_debug(0);
  
  printf("Setting off interrupt generation...\n"); 
  
  // Toggle the GPIO to generate interrupts
  for (i=0; i<10000; i++)
  {
    libsoc_gpio_set_level(gpio_output, HIGH);
    libsoc_gpio_set_level(gpio_output, LOW);
    
    // Let the other thread get a look in otherwise interrupt success is
    // ~85%, with usleep(1) success is nearly always %100  with no other load.
    usleep(1);
  }
  
  // Turn debug back on
  libsoc_set_debug(1);
  
  printf("Caught %d of 10000 interrupts\n", interrupt_count); 
  
  // Cancel the callback on interrupt
  libsoc_gpio_callback_interrupt_cancel(gpio_input);
  
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
