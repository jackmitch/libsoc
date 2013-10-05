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
#define READ  0x03
#define RDSR  0x05

int main()
{
  libsoc_set_debug(1);
   
  spi* spi_dev = libsoc_spi_init(SPI_DEVICE, CHIP_SELECT);
   
  uint8_t tx[4], rx[4];

  libsoc_spi_set_mode(spi_dev, MODE_0);
  libsoc_spi_get_mode(spi_dev);
  
  libsoc_spi_set_speed(spi_dev, 1000000);
  libsoc_spi_get_speed(spi_dev);
  
  libsoc_spi_set_bits_per_word(spi_dev, BITS_8);
  libsoc_spi_get_bits_per_word(spi_dev);
  
  printf("Reading STATUS register\n");
  
  tx[0] = RDSR;
  tx[1] = 0;
  rx[0] = 0;
  rx[1] = 0;
  
  libsoc_spi_rw(spi_dev, tx, rx, 2);
  
  printf("STATUS is 0x%02x\n", rx[1]);
  
  tx[0] = WREN;
  
  printf("Setting write enable bit\n");
   
  libsoc_spi_write(spi_dev, tx, 1);
  
  tx[0] = RDSR;
  tx[1] = 0;
  rx[0] = 0;
  rx[1] = 0;
  
  printf("Reading STATUS register\n");
  
  libsoc_spi_rw(spi_dev, tx, rx, 2);
  
  printf("STATUS is 0x%02x\n", rx[1]);
  
  if (!(rx[1] & 0x02)) {
    printf("Write Enable was not set, failure\n");
    goto free;
  }
   
  tx[0] = WRITE;
  tx[1] = 0;
  tx[2] = 0;
  tx[3] = 0xAB;
  
  printf("Writing 0xAB to page address 0x0000\n");
   
  libsoc_spi_write(spi_dev, tx, 4);
  
  do {
  
    tx[0] = RDSR;
    tx[1] = 0;
    rx[0] = 0;
    rx[1] = 0;
  
    libsoc_spi_rw(spi_dev, tx, rx, 2);
    
    if (rx[1] & 0x01)
      printf("Write in progress...\n");
    
  } while (rx[1] & 0x1);
  
  tx[0] = READ;
  tx[1] = 0;
  tx[2] = 0;
  tx[3] = 0;
  
  rx[0] = 0;
  rx[1] = 0;
  rx[2] = 0;
  rx[3] = 0;
  
  printf("Reading page address 0x0000\n");
  
  libsoc_spi_rw(spi_dev, tx, rx, 4);
  
  if (rx[3] == 0xAB) {
    printf("Read 0xAB, success!\n");
  }
  else {
    printf("Read 0x%02x, failure!\n", rx[3]);
  }
  
  free:
  
  libsoc_spi_free(spi_dev);

  return EXIT_SUCCESS;
}
