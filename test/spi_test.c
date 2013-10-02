#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdint.h>

#include "libsoc_spi.h"
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
 
#define SPI_DEVICE   1
#define CHIP_SELECT  0

#define WREN  0x06
#define WRDI  0x04
#define WRITE 0x02
#define READ 0x03
#define RDSR 0x05

int main()
{
  libsoc_set_debug(1);
   
  spi* spi_dev = libsoc_spi_init(SPI_DEVICE, CHIP_SELECT);
   
  uint8_t tx[4], rx[4];
  
  libsoc_spi_set_mode(spi_dev, 0);
  libsoc_spi_set_speed(spi_dev, 1000000);
  libsoc_spi_set_bits_per_word(spi_dev, 8);
  
  tx[0] = RDSR;
  tx[1] = 0;
  rx[0] = 0;
  rx[1] = 0;
  
  libsoc_spi_rw(spi_dev, tx, rx, 2);
  
  tx[0] = WREN;
   
  libsoc_spi_write(spi_dev, tx, 1);
  
  tx[0] = RDSR;
  tx[1] = 0;
  rx[0] = 0;
  rx[1] = 0;
  
  libsoc_spi_rw(spi_dev, tx, rx, 2);
   
  tx[0] = WRITE;
  tx[1] = 0;
  tx[2] = 0;
  tx[3] = 0xAB;
   
  libsoc_spi_write(spi_dev, tx, 4);
  
  do {
    
    usleep(1);
  
    tx[0] = RDSR;
    tx[1] = 0;
    rx[0] = 0;
    rx[1] = 0;
  
    libsoc_spi_rw(spi_dev, tx, rx, 2);
    
  } while (rx[1] & 0x1);
  
  tx[0] = READ;
  tx[1] = 0;
  tx[2] = 0;
  tx[3] = 0;
  
  rx[0] = 0;
  rx[1] = 0;
  rx[2] = 0;
  rx[3] = 0;
  
  libsoc_spi_rw(spi_dev, tx, rx, 4);
  
  if (rx[3] == 0xAB) {
    printf("Success!\n");
  }
  else {
    printf("Failure!\n");
  }
  
  libsoc_spi_free(spi_dev);

  return EXIT_SUCCESS;
}
