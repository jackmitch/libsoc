#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "libsoc_spi.h"

spi* libsoc_spi_init (int spidev_device, int chip_select)
{
  spi* spi = malloc(sizeof(spi));
  
  sprintf(spi->path, "/dev/spidev%d.%d", spidev_device, chip_select);
  
  if (file_valid(spi->path))
    return spi;
    
  libsoc_debug(__func__, "spidev device %s does not exist", spi->path);  
  
  free(spi);
  
  return NULL;
}



int libsoc_spi_free(spi* spi)
{
  if (spi == NULL)
    return EXIT_FAILURE;
    
  free(spi);
}
