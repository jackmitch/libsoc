#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>

#include "libsoc_i2c.h"
#include "libsoc_debug.h"
#include "libsoc_file.h"

inline void
libsoc_i2c_debug (const char *func, i2c * i2c, char *format, ...)
{
#ifdef DEBUG

  if (libsoc_get_debug ())
    {
      va_list args;

      fprintf (stderr, "libsoc-i2c-debug: ");

      va_start (args, format);
      vfprintf (stderr, format, args);
      va_end (args);

      if (i2c == NULL)
	{
	  fprintf (stderr, " (NULL, %s)", func);
	}
      else
	{
	  fprintf (stderr, " (i2c-%d, %d, %s)", i2c->bus,
		   i2c->address, func);
	}

      fprintf (stderr, "\n");
    }
#endif
}

i2c *
libsoc_i2c_init (uint8_t i2c_bus, uint8_t i2c_address)
{
  i2c *i2c_dev;

  libsoc_i2c_debug (__func__, NULL, "initialising i2c bus %d at "
                      "address %d", i2c_bus, i2c_address);

  i2c_dev = malloc (sizeof (i2c));

  if (i2c_dev == NULL)
    {
      libsoc_i2c_debug (__func__, NULL, "failed to allocate memory");
      return NULL;
    }

  char path[40];

  i2c_dev->bus = i2c_bus;
  i2c_dev->address = i2c_address;

  sprintf (path, "/dev/i2c-%d", i2c_dev->bus);

  if (!file_valid (path))
    {
      libsoc_i2c_debug (__func__, i2c_dev, "%s not a vaild device", path);
      goto error;
    }

  i2c_dev->fd = file_open (path, O_SYNC | O_RDWR);

  if (i2c_dev->fd < 0)
    {
      libsoc_i2c_debug (__func__, i2c_dev, "%s could not be opened", path);
      goto error;
    }

  return i2c_dev;

error:

  free (i2c_dev);

  return NULL;
}

int
libsoc_i2c_free (i2c * i2c)
{
  if (i2c == NULL)
    {
      libsoc_i2c_debug (__func__, NULL, "i2c was not valid");
      return EXIT_FAILURE;
    }

  libsoc_i2c_debug (__func__, i2c, "freeing i2c device");

  free (i2c);

  return EXIT_SUCCESS;
}

int 
libsoc_i2c_ioctl(i2c * i2c, int num_messages)
{
   i2c->packets.msgs = i2c->messages;
   i2c->packets.nmsgs = num_messages;

   if (ioctl(i2c->fd, I2C_RDWR, &i2c->packets) < 0)
   {
      libsoc_i2c_debug(__func__, i2c, "message failed");
      perror ("libsoc-i2c-debug");
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}

int
libsoc_i2c_set_timeout(i2c * i2c, int timeout)
{
   if (ioctl(i2c->fd, I2C_TIMEOUT, timeout) < 0)
   {
      libsoc_i2c_debug(__func__, i2c, "setting timeout failed");
      perror ("libsoc-i2c-debug");
      return EXIT_FAILURE;
   }
   
   libsoc_i2c_debug(__func__, i2c, "timeout set to %dms", (timeout*10));
   
   return EXIT_SUCCESS;
}

int 
libsoc_i2c_write (i2c * i2c, uint8_t * buffer, uint16_t len)
{ 
    if (i2c == NULL || buffer == NULL)
    {
      libsoc_i2c_debug (__func__, i2c, "i2c | buffer was NULL");
      return EXIT_FAILURE;
    }
    
      libsoc_i2c_debug(__func__, i2c, "Writing buffer of length %d", len);
  
   i2c->messages[0].addr = i2c->address;
   i2c->messages[0].flags = 0;
   i2c->messages[0].len = len;
   i2c->messages[0].buf = buffer;

   return libsoc_i2c_ioctl(i2c, 1);
}

int 
libsoc_i2c_read (i2c * i2c, uint8_t * buffer, uint16_t len)
{ 
   if (i2c == NULL || buffer == NULL)
    {
      libsoc_i2c_debug (__func__, i2c, "i2c | buffer was NULL");
      return EXIT_FAILURE;
    }
    
    libsoc_i2c_debug(__func__, i2c, "Reading buffer of length %d", len);
  
   i2c->messages[0].addr = i2c->address;
   i2c->messages[0].flags = I2C_M_RD;
   i2c->messages[0].len = len;
   i2c->messages[0].buf = buffer;

   return libsoc_i2c_ioctl(i2c, 1);
}

