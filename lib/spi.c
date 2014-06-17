#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "libsoc_spi.h"
#include "libsoc_debug.h"
#include "libsoc_file.h"

inline void
libsoc_spi_debug (const char *func, spi * spi, char *format, ...)
{
#ifdef DEBUG

  if (libsoc_get_debug ())
    {
      va_list args;

      fprintf (stderr, "libsoc-spi-debug: ");

      va_start (args, format);
      vfprintf (stderr, format, args);
      va_end (args);

      if (spi == NULL)
	{
	  fprintf (stderr, " (NULL, %s)", func);
	}
      else
	{
	  fprintf (stderr, " (spidev%d.%d, %s)", spi->spi_dev,
		   spi->chip_select, func);
	}

      fprintf (stderr, "\n");
    }
#endif
}

spi *
libsoc_spi_init (uint8_t spidev_device, uint8_t chip_select)
{
  spi *spi_dev;

  libsoc_spi_debug (__func__, NULL, "initialising spidev device %d.%d",
		    spidev_device, chip_select);

  spi_dev = malloc (sizeof (spi));

  if (spi_dev == NULL)
    {
      libsoc_spi_debug (__func__, NULL, "failed to allocate memory");
      return NULL;
    }

  char path[40];

  spi_dev->spi_dev = spidev_device;
  spi_dev->chip_select = chip_select;

  sprintf (path, "/dev/spidev%d.%d", spi_dev->spi_dev, spi_dev->chip_select);

  if (!file_valid (path))
    {
      libsoc_spi_debug (__func__, spi_dev, "%s not a vaild device", path);
      goto error;
    }

  spi_dev->fd = file_open (path, O_SYNC | O_RDWR);

  if (spi_dev->fd < 0)
    {
      libsoc_spi_debug (__func__, spi_dev, "%s could not be opened", path);
      goto error;
    }

  return spi_dev;

error:

  free (spi_dev);

  return NULL;
}

int
libsoc_spi_set_bits_per_word (spi * spi, spi_bpw bpw)
{
  if (bpw != BITS_8 && bpw != BITS_16)
    {
      libsoc_spi_debug (__func__, spi, "bits per word was not BITS_8"
			" or BITS_16", bpw);
      return EXIT_FAILURE;
    }

  libsoc_spi_debug (__func__, spi, "setting bits per word to %d", bpw);

  int ret = ioctl (spi->fd, SPI_IOC_WR_BITS_PER_WORD, &bpw);

  if (ret == -1)
    {
      libsoc_spi_debug (__func__, spi, "failed setting bits per word");
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

spi_bpw
libsoc_spi_get_bits_per_word (spi * spi)
{
  uint8_t bpw;

  int ret = ioctl (spi->fd, SPI_IOC_RD_BITS_PER_WORD, &bpw);

  if (ret == -1)
    {
      libsoc_spi_debug (__func__, spi, "failed reading bits per word");
      return BPW_ERROR;
    }

  switch (bpw)
    {
    case 8:
      libsoc_spi_debug (__func__, spi, "read bits per word as 8");
      return BITS_8;
    case 16:
      libsoc_spi_debug (__func__, spi, "read bits per word as 16");
      return BITS_16;
    default:
      libsoc_spi_debug (__func__, spi, "bits per word not recognised");
      return BPW_ERROR;
    }
}

int
libsoc_spi_set_speed (spi * spi, uint32_t speed)
{
  libsoc_spi_debug (__func__, spi, "setting speed to %dHz", speed);

  int ret = ioctl (spi->fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);

  if (ret == -1)
    {
      libsoc_spi_debug (__func__, spi, "failed setting speed");
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

uint32_t
libsoc_spi_get_speed (spi * spi)
{
  uint32_t speed;

  int ret = ioctl (spi->fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);

  if (ret == -1)
    {
      libsoc_spi_debug (__func__, spi, "failed reading speed");
      return -1;
    }

  libsoc_spi_debug (__func__, spi, "read speed as %dHz", speed);

  return speed;
}

int
libsoc_spi_set_mode (spi * spi, spi_mode mode)
{
  if (spi == NULL)
    {
      libsoc_spi_debug (__func__, NULL, "spi was not valid");
      return EXIT_FAILURE;
    }

  libsoc_spi_debug (__func__, spi, "setting mode to %d", mode);

  uint8_t new_mode;

  switch (mode)
    {
    case MODE_0:
      new_mode = SPI_MODE_0;
      break;
    case MODE_1:
      new_mode = SPI_MODE_1;
      break;
    case MODE_2:
      new_mode = SPI_MODE_2;
      break;
    case MODE_3:
      new_mode = SPI_MODE_3;
      break;
    default:
      return EXIT_FAILURE;
    }

  int ret = ioctl (spi->fd, SPI_IOC_WR_MODE, &new_mode);

  if (ret == -1)
    {
      libsoc_spi_debug (__func__, spi, "failed setting mode");
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

spi_mode
libsoc_spi_get_mode (spi * spi)
{
  uint8_t mode;

  if (spi == NULL)
    {
      libsoc_spi_debug (__func__, NULL, "spi was not valid");
      return EXIT_FAILURE;
    }

  int ret = ioctl (spi->fd, SPI_IOC_RD_MODE, &mode);

  if (ret == -1)
    {
      libsoc_spi_debug (__func__, spi, "failed reading mode");
      return MODE_ERROR;
    }

  switch (mode)
    {
    case SPI_MODE_0:
      libsoc_spi_debug (__func__, spi, "read mode as 0");
      return MODE_0;
    case SPI_MODE_1:
      libsoc_spi_debug (__func__, spi, "read mode as 1");
      return MODE_1;
    case SPI_MODE_2:
      libsoc_spi_debug (__func__, spi, "read mode as 2");
      return MODE_2;
    case SPI_MODE_3:
      libsoc_spi_debug (__func__, spi, "read mode as 3");
      return MODE_3;
    default:
      libsoc_spi_debug (__func__, spi, "mode not recognised");
      return MODE_ERROR;
    }
}

int
libsoc_spi_write (spi * spi, uint8_t * tx, uint32_t len)
{
  libsoc_spi_debug (__func__, spi, "performing write transfer of %d bytes",
		    len);

  if (spi == NULL || tx == NULL)
    {
      libsoc_spi_debug (__func__, spi, "spi or tx was NULL");
      return EXIT_FAILURE;
    }

  if (len <= 0)
    {
      libsoc_spi_debug (__func__, spi, "length was less than zero");
      return EXIT_FAILURE;
    }

  int ret;

  struct spi_ioc_transfer tr = {
    .tx_buf = (unsigned long) tx,
    .len = len,
  };

  ret = ioctl (spi->fd, SPI_IOC_MESSAGE (1), &tr);

  if (ret < 1)
  {
    libsoc_spi_debug (__func__, spi, "failed sending message");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int
libsoc_spi_read (spi * spi, uint8_t * rx, uint32_t len)
{
  libsoc_spi_debug (__func__, spi, "performing read transfer of %d bytes",
		    len);

  if (spi == NULL || rx == NULL)
    {
      libsoc_spi_debug (__func__, spi, "spi or rx was NULL");
      return EXIT_FAILURE;
    }

  if (len <= 0) 
    {
      libsoc_spi_debug (__func__, spi, "length was less than zero");
      return EXIT_FAILURE;
    }

  int ret;

  struct spi_ioc_transfer tr = {
    .rx_buf = (unsigned long) rx,
    .len = len,
  };

  ret = ioctl (spi->fd, SPI_IOC_MESSAGE (1), &tr);

  if (ret < 1)
    {
      libsoc_spi_debug (__func__, spi, "failed recieving message");
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

int
libsoc_spi_rw (spi * spi, uint8_t * tx, uint8_t * rx, uint32_t len)
{
  libsoc_spi_debug (__func__, spi,
		    "performing duplex rw transfer of %d bytes", len);

  if (spi == NULL || rx == NULL || tx == NULL)
    {
      libsoc_spi_debug (__func__, spi, "spi | rx | tx was NULL");
      return EXIT_FAILURE;
    }

  if (len <= 0)
    {
      libsoc_spi_debug (__func__, spi, "length was less than zero");
      return EXIT_FAILURE;
    }

  int ret;

  struct spi_ioc_transfer tr = {
    .rx_buf = (unsigned long) rx,
    .tx_buf = (unsigned long) tx,
    .len = len,
  };

  ret = ioctl (spi->fd, SPI_IOC_MESSAGE (1), &tr);

  if (ret < 1)
  {
    libsoc_spi_debug (__func__, spi, "failed duplex transfer");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int
libsoc_spi_free (spi * spi)
{
  if (spi == NULL)
    {
      libsoc_spi_debug (__func__, NULL, "spi was not valid");
      return EXIT_FAILURE;
    }

  libsoc_spi_debug (__func__, spi, "freeing spi device");

  free (spi);

  return EXIT_SUCCESS;
}
