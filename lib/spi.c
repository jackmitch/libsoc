#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "libsoc_spi.h"

spi* libsoc_spi_init (int spidev_device, int chip_select)
{
  spi* spi = malloc(sizeof(spi));
  char path[40];
  
  sprintf(path, "/dev/spidev%d.%d", spidev_device, chip_select);
  
  if (!file_valid(path))
    goto error;
  
  spi->fd = file_open(path);
    
  if (spi->fd < 0)
    goto error;
    
  return spi;
    
  error:
    
  libsoc_debug(__func__, "spidev device %s does not exist", path);  
  
  free(spi);
  
  return NULL;
}

int libsoc_spi_set_bits_per_word(spi* spi, uint8_t bpw)
{
  if (bpw != 8 || bpw != 16)
    return EXIT_FAILURE;
  
  int ret = ioctl(spi->fd, SPI_IOC_WR_BITS_PER_WORD, &bpw);
	
  if (ret == -1)
		libsoc_debug(__func__, "Setting bits per word failed");
    return EXIT_FAILURE;
    
  return EXIT_SUCCESS;
}

int libsoc_spi_set_speed(spi* spi, uint32_t speed)
{  
  int ret = ioctl(spi->fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	
  if (ret == -1)
		libsoc_debug(__func__, "Setting speed failed");
    return EXIT_FAILURE;
    
  return EXIT_SUCCESS;
}

int libsoc_spi_set_mode(spi* spi, uint8_t mode)
{
  switch(mode) {
    case 0:
      mode = SPI_MODE_0;
      break;
    case 1:
      mode = SPI_MODE_1;
      break;
    case 2:
      mode = SPI_MODE_2;
      break;
    case 3:
      mode = SPI_MODE_3;
      break;
    default:
      return EXIT_FAILURE;
  }
  
  int ret = ioctl(spi->fd, SPI_IOC_WR_MODE, &mode);
	
  if (ret == -1)
		libsoc_debug(__func__, "Setting bits per word failed");
    return EXIT_FAILURE;
    
  return EXIT_SUCCESS;
}

int libsoc_spi_write(spi* spi, uint8_t* tx, uint32_t len)
{
  if (spi == NULL || tx == NULL)
    return EXIT_FAILURE;
    
  if (len <= 0)
    return EXIT_FAILURE;
  
  int ret;
  
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.len = len,
	};

	ret = ioctl(spi->fd, SPI_IOC_MESSAGE(1), &tr);
  
	if (ret < 1)
		libsoc_debug(__func__, "Failed to send message");
  
}

int libsoc_spi_read(spi* spi, uint8_t* rx, uint32_t len)
{
  if (spi == NULL || rx == NULL)
    return EXIT_FAILURE;
    
  if (len <= 0)
    return EXIT_FAILURE;
  
  int ret;
  
	struct spi_ioc_transfer tr = {
		.rx_buf = (unsigned long)rx,
		.len = len,
	};

	ret = ioctl(spi->fd, SPI_IOC_MESSAGE(1), &tr);
  
	if (ret < 1)
		libsoc_debug(__func__, "Failed to recieve message");
  
}

int libsoc_spi_rw(spi* spi, uint8_t* tx, uint8_t* rx, uint32_t len)
{
  if (spi == NULL || rx == NULL || tx == NULL)
    return EXIT_FAILURE;
    
  if (len <= 0)
    return EXIT_FAILURE;
  
  int ret;
  
	struct spi_ioc_transfer tr = {
		.rx_buf = (unsigned long)rx,
    .tx_buf = (unsigned long)tx,
		.len = len,
	};

	ret = ioctl(spi->fd, SPI_IOC_MESSAGE(1), &tr);
  
	if (ret < 1)
		libsoc_debug(__func__, "Failed to recieve message");
  
}

int libsoc_spi_free(spi* spi)
{
  if (spi == NULL)
    return EXIT_FAILURE;
    
  free(spi);
}
