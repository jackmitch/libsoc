#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>

#include "libsoc_i2c.h"
#include "libsoc_debug.h"

/**
 * 
 * This i2c_test is intended to be run on beaglebone white hardware
 * and uses the I2C2 which is device id 1 by default, as not all 3 I2C
 * busses are actiavted, and i2c devices are enumerated by amount.
 * 
 * The i2c bus is on pins P9_19 as SCL, P9_20 as SDA
 *
 * The BeagleBone is connected to a ST MicroElectronics M24C01-WBN6P 1K 
 * EEPROM
 *
 * The test covers writing 32 bytes of random data, to a random page
 * on the EEPROM. It then reads the page back, and compares the data
 * read against the data sent, the test passes if all data matches.
 * 
 */
 
#define I2C_BUS   1
#define ADDRESS   0b1010000

#define EEPROM_SIZE 128*8
#define EEPROM_PAGE_SIZE 16
#define EEPROM_NUM_PAGES EEPROM_SIZE/EEPROM_PAGE_SIZE

int main()
{
  libsoc_set_debug(1);
   
  i2c *eeprom = libsoc_i2c_init(I2C_BUS, ADDRESS);
  
  if (eeprom == NULL) {
    printf("Failed to get I2C device!\n");
    return EXIT_FAILURE;
  }
  
  libsoc_i2c_set_timeout(eeprom, 1);
  
  struct timeval t1;
  gettimeofday(&t1, NULL);
  srand(t1.tv_usec * t1.tv_sec);
  
  uint8_t data[EEPROM_PAGE_SIZE+1];
  uint8_t data_read[EEPROM_PAGE_SIZE];
  uint8_t page = (rand() % EEPROM_NUM_PAGES) * EEPROM_PAGE_SIZE;
  int i, ret;
  
  data[0] = page;
  
  for (i=1; i<(EEPROM_PAGE_SIZE+1); i++) {
    data[i] = rand() % 255;
  }
  
  // Write page
  printf ("Writing to page starting at byte address: %d\n", page);
  ret = libsoc_i2c_write(eeprom, data, EEPROM_PAGE_SIZE+1);
  
  printf("Waiting for data to be written\n");
  
  while (libsoc_i2c_write(eeprom, &page, 1) == EXIT_FAILURE) {
    printf("Waiting...\n");
    usleep(1);
  }
  
  printf("Data sucessfully written!\n");
  
  printf("Reading data\n");
  
  // Set byte address to read
  ret = libsoc_i2c_write(eeprom, &page, 1);
  
  // Read page
  ret = libsoc_i2c_read(eeprom, data_read, EEPROM_PAGE_SIZE);
  
  if (ret != EXIT_SUCCESS) {
    printf("Read page failed\n");
  }
  
  printf ("Reading page starting at byte address: %d\n", page);
  
  for (i=0; i<EEPROM_PAGE_SIZE; i++) {
    printf("data[%d] = 0x%02x : data_read[%d] = 0x%02x", i, data[(i+1)], i, data_read[i]);
    
    if (data[(i+1)] == data_read[i]) {
      printf(" : Correct\n");
    } else {
      printf(" : Incorrect\n");
    }
  }
  
  goto free;

  free:
  
  libsoc_i2c_free(eeprom);

  return EXIT_SUCCESS;
}
