/* SPDX-License-Identifier: LGPL-2.1-only */

#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <poll.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "libsoc_file.h"
#include "libsoc_debug.h"
#include "libsoc_gpio.h"

#define STR_BUF 256

const char gpio_level_strings[2][STR_BUF] = { "0", "1" };
const char gpio_direction_strings[2][STR_BUF] = { "in", "out" };
const char gpio_edge_strings[4][STR_BUF] = { "rising", "falling", "none", "both" };

void
libsoc_gpio_debug (const char *func, int gpio, char *format, ...)
{
#ifdef DEBUG

  if (libsoc_get_debug ())
    {
      va_list args;

      fprintf (stderr, "libsoc-gpio-debug: ");

      va_start (args, format);
      vfprintf (stderr, format, args);
      va_end (args);

      if (gpio >= 0)
	{
	  fprintf (stderr, " (%d, %s)", gpio, func);
	}
      else
	{
	  fprintf (stderr, " (NULL, %s)", func);
	}

      fprintf (stderr, "\n");
    }
#endif
}

gpio *
libsoc_gpio_request (unsigned int gpio_id, gpio_mode mode)
{
  gpio *new_gpio;
  char tmp_str[STR_BUF];
  int shared = 0;

  if (mode != LS_GPIO_SHARED && mode != LS_GPIO_GREEDY && mode != LS_GPIO_WEAK)
    {
      libsoc_gpio_debug (__func__, gpio_id,
			 "mode was not set, or invalid,"
			 " setting mode to LS_GPIO_SHARED");
      mode = LS_GPIO_SHARED;
    }

  libsoc_gpio_debug (__func__, gpio_id, "requested gpio");

  sprintf (tmp_str, "/sys/class/gpio/gpio%d/value", gpio_id);

  if (file_valid (tmp_str))
    {
      libsoc_gpio_debug (__func__, gpio_id, "GPIO already exported");

      switch (mode)
	{
	case LS_GPIO_WEAK:
	  {
	    return NULL;
	  }

	case LS_GPIO_SHARED:
	  {
	    shared = 1;
	    break;
	  }

	default:
	  {
	    break;
	  }
	}
    }
  else
    {
      int fd = file_open ("/sys/class/gpio/export", O_SYNC | O_WRONLY);

      if (fd < 0)
	return NULL;

      sprintf (tmp_str, "%d", gpio_id);

      if (file_write (fd, tmp_str, STR_BUF) < 0)
	return NULL;

      if (file_close (fd))
	return NULL;

      sprintf (tmp_str, "/sys/class/gpio/gpio%d", gpio_id);

      if (!file_valid (tmp_str))
	{
	  libsoc_gpio_debug (__func__, gpio_id,
			     "gpio did not export correctly");
	  perror ("libsoc-gpio-debug");
	  return NULL;
	}
    }

  new_gpio = malloc (sizeof (gpio));
  if (new_gpio == NULL)
    return NULL;

  sprintf (tmp_str, "/sys/class/gpio/gpio%d/value", gpio_id);

  new_gpio->value_fd = file_open (tmp_str, O_SYNC | O_RDWR);

  if (new_gpio->value_fd < 0)
    {
      free(new_gpio);
      return NULL;
    }

  new_gpio->gpio = gpio_id;
  new_gpio->shared = shared;
  new_gpio->callback = NULL;

  // Set up a pollfd in case we are used for polling later
  new_gpio->pfd.fd = new_gpio->value_fd;
  new_gpio->pfd.events = POLLPRI;
  new_gpio->pfd.revents = 0;

  return new_gpio;
}

int
libsoc_gpio_free (gpio * gpio)
{
  char tmp_str[STR_BUF];
  int fd;

  if (gpio == NULL)
    {
      libsoc_gpio_debug (__func__, -1, "invalid gpio pointer");
      return EXIT_FAILURE;
    }

  libsoc_gpio_debug (__func__, gpio->gpio, "freeing gpio");

  if (gpio->callback != NULL)
    {
      printf ("Freeing callback!\n");
      // Turn off the callback if there is one enabled
      libsoc_gpio_callback_interrupt_cancel (gpio);
    }

  if (file_close (gpio->value_fd) < 0)
    return EXIT_FAILURE;

  if (gpio->shared == 1)
    {
      free (gpio);
      return EXIT_SUCCESS;
    }

  fd = file_open ("/sys/class/gpio/unexport", O_SYNC | O_WRONLY);

  if (fd < 0)
    return EXIT_FAILURE;

  sprintf (tmp_str, "%d", gpio->gpio);

  if (file_write (fd, tmp_str, STR_BUF) < 0)
    return EXIT_FAILURE;

  if (file_close (fd) < 0)
    return EXIT_FAILURE;

  sprintf (tmp_str, "/sys/class/gpio/gpio%d", gpio->gpio);

  if (file_valid (tmp_str))
    {
      libsoc_gpio_debug (__func__, gpio->gpio, "freeing failed");
      return EXIT_FAILURE;
    }

  free (gpio);

  return EXIT_SUCCESS;
}

int
libsoc_gpio_set_direction (gpio * current_gpio, gpio_direction direction)
{
  int fd;
  char path[STR_BUF];

  if (current_gpio == NULL)
    {
      libsoc_gpio_debug (__func__, -1, "invalid gpio pointer");
      return EXIT_FAILURE;
    }

  libsoc_gpio_debug (__func__, current_gpio->gpio,
		     "setting direction to %s",
		     gpio_direction_strings[direction]);

  sprintf (path, "/sys/class/gpio/gpio%d/direction", current_gpio->gpio);

  fd = file_open (path, O_SYNC | O_WRONLY);

  if (fd < 0)
    return EXIT_FAILURE;

  if (file_write (fd, gpio_direction_strings[direction], STR_BUF) < 0)
    return EXIT_FAILURE;

  if (file_close (fd) < 0)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}

gpio_direction
libsoc_gpio_get_direction (gpio * current_gpio)
{
  int fd;
  char tmp_str[STR_BUF];

  if (current_gpio == NULL)
    {
      libsoc_gpio_debug (__func__, -1, "invalid gpio pointer");
      return DIRECTION_ERROR;
    }

  sprintf (tmp_str, "/sys/class/gpio/gpio%d/direction", current_gpio->gpio);

  fd = file_open (tmp_str, O_RDONLY);

  if (fd < 0)
    return DIRECTION_ERROR;

  lseek (fd, 0, SEEK_SET);

  if (file_read (fd, tmp_str, STR_BUF) < 0)
    return DIRECTION_ERROR;

  if (file_close (fd) < 0)
    return DIRECTION_ERROR;

  if (strncmp (tmp_str, "in", 2) <= 0)
    {
      libsoc_gpio_debug (__func__, current_gpio->gpio,
			 "read direction as input");
      return INPUT;
    }
  else
    {
      libsoc_gpio_debug (__func__, current_gpio->gpio,
			 "read direction as output");
      return OUTPUT;
    }
}

int
libsoc_gpio_set_level (gpio * current_gpio, gpio_level level)
{
  if (current_gpio == NULL)
    {
      libsoc_gpio_debug (__func__, -1, "invalid gpio pointer");
      return EXIT_FAILURE;
    }

  libsoc_gpio_debug (__func__, current_gpio->gpio, "setting level to %d",
		     level);

  if (file_write (current_gpio->value_fd, gpio_level_strings[level], 1) < 0)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}

gpio_level
libsoc_gpio_get_level (gpio * current_gpio)
{
  char level[2];

  if (current_gpio == NULL)
    {
      libsoc_gpio_debug (__func__, -1, "invalid gpio pointer");
      return LEVEL_ERROR;
    }

  lseek (current_gpio->value_fd, 0, SEEK_SET);

  if (read (current_gpio->value_fd, level, 2) != 2)
  {
    libsoc_gpio_debug (__func__, current_gpio->gpio, "level read failed");
    perror ("libgpio");
    return LEVEL_ERROR;
  }

  if (level[0] == '0')
    {
      libsoc_gpio_debug (__func__, current_gpio->gpio, "read level as low");
      return LOW;
    }
  else
    {
      libsoc_gpio_debug (__func__, current_gpio->gpio, "read level as high");
      return HIGH;
    }
}

int
libsoc_gpio_set_edge (gpio * current_gpio, gpio_edge edge)
{
  int fd;
  char path[STR_BUF];

  if (current_gpio == NULL)
    {
      libsoc_gpio_debug (__func__, -1, "invalid gpio pointer");
      return EXIT_FAILURE;
    }

  libsoc_gpio_debug (__func__, current_gpio->gpio, "setting edge to %s",
		     gpio_edge_strings[edge]);

  sprintf (path, "/sys/class/gpio/gpio%d/edge", current_gpio->gpio);

  fd = file_open (path, O_SYNC | O_WRONLY);

  if (fd < 0)
    return EXIT_FAILURE;

  if (file_write (fd, gpio_edge_strings[edge], STR_BUF) < 0)
    return EXIT_FAILURE;

  if (file_close (fd) < 0)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}

gpio_edge
libsoc_gpio_get_edge (gpio * current_gpio)
{
  int fd;
  char tmp_str[STR_BUF];

  if (current_gpio == NULL)
    {
      libsoc_gpio_debug (__func__, -1, "invalid gpio pointer");
      return EDGE_ERROR;
    }

  sprintf (tmp_str, "/sys/class/gpio/gpio%d/edge", current_gpio->gpio);

  fd = file_open (tmp_str, O_RDONLY);

  if (fd < 0)
    return EDGE_ERROR;

  lseek (fd, 0, SEEK_SET);

  if (file_read (fd, tmp_str, STR_BUF) < 0)
    return EDGE_ERROR;

  if (file_close (fd) < 0)
    return EDGE_ERROR;

  if (strncmp (tmp_str, "r", 1) == 0)
    {
      libsoc_gpio_debug (__func__, current_gpio->gpio, "read edge as rising");
      return RISING;
    }
  else if (strncmp (tmp_str, "f", 1) == 0)
    {
      libsoc_gpio_debug (__func__, current_gpio->gpio,
			 "read edge as falling");
      return FALLING;
    }
  else if (strncmp (tmp_str, "b", 1) == 0)
    {
      libsoc_gpio_debug (__func__, current_gpio->gpio,
			 "read edge as both");
      return BOTH;
    }
  else
    {
      libsoc_gpio_debug (__func__, current_gpio->gpio, "read edge as none");
      return NONE;
    }
}

int
libsoc_gpio_poll (gpio * gpio, int timeout)
{
  int rc;
  char c;

  // do an initial read to clear interrupt,
  rc = lseek(gpio->value_fd, 0, SEEK_SET);
  rc = read(gpio->value_fd, &c, 1);

  rc = poll(&gpio->pfd, 1, timeout);
  if (rc == -1)
    {
      libsoc_gpio_debug (__func__, gpio->gpio, "poll failed");
      perror ("libsoc-gpio-debug");
      return LS_INT_ERROR;
    }
  else if (rc == 1 && gpio->pfd.revents & POLLPRI)
    {
      // do a final read to clear interrupt
      rc = read(gpio->value_fd, &c, 1);
      return LS_INT_TRIGGERED;
    }
  return LS_INT_TIMEOUT;
}

int
libsoc_gpio_wait_interrupt (gpio * gpio, int timeout)
{
  if (gpio == NULL)
    {
      libsoc_gpio_debug (__func__, -1, "invalid gpio pointer");
      return LS_INT_ERROR;
    }

  if (libsoc_gpio_get_direction (gpio) != INPUT)
    {
      libsoc_gpio_debug (__func__, gpio->gpio, "gpio is not set as input");
      return LS_INT_ERROR;
    }

  gpio_edge test_edge = libsoc_gpio_get_edge (gpio);

  if (test_edge == EDGE_ERROR || test_edge == NONE)
    {
      libsoc_gpio_debug (__func__, gpio->gpio,
			 "edge must be FALLING, RISING or BOTH");
      return LS_INT_ERROR;
    }

  return libsoc_gpio_poll(gpio, timeout);
}

void *
__libsoc_new_interrupt_callback_thread (void *void_gpio)
{
  gpio *gpio = void_gpio;

  pthread_mutex_unlock(&gpio->callback->ready);

  while (1)
    {
      if (libsoc_gpio_poll(gpio, -1) == LS_INT_TRIGGERED)
        {
          libsoc_gpio_debug (__func__, gpio->gpio, "caught interrupt");
          gpio->callback->callback_fn (gpio->callback->callback_arg);
        }
    }
}

int
libsoc_gpio_callback_interrupt (gpio * gpio, int (*callback_fn) (void *),
				void *arg)
{
  pthread_t *poll_thread = malloc (sizeof (pthread_t));
  pthread_attr_t pthread_attr;

  pthread_attr_init (&pthread_attr);
  pthread_attr_setschedpolicy (&pthread_attr, SCHED_FIFO);

  struct gpio_callback *new_gpio_callback;

  libsoc_gpio_debug (__func__, gpio->gpio, "creating new callback");

  new_gpio_callback = malloc (sizeof (struct gpio_callback));

  new_gpio_callback->callback_fn = callback_fn;
  new_gpio_callback->callback_arg = arg;
  new_gpio_callback->thread = poll_thread;

  gpio->callback = new_gpio_callback;

  pthread_mutex_init(&new_gpio_callback->ready, NULL);
  pthread_mutex_lock(&new_gpio_callback->ready);

  int ret = pthread_create (poll_thread, NULL,
			    __libsoc_new_interrupt_callback_thread, gpio);

  if (ret == 0)
    {
      // Wait for thread to be initialised and ready
      pthread_mutex_lock(&new_gpio_callback->ready);
    }
  else
    {
      pthread_mutex_unlock(&gpio->callback->ready);
      pthread_mutex_destroy(&gpio->callback->ready);
      free (gpio->callback->thread);
      free (gpio->callback);

      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

int
libsoc_gpio_callback_interrupt_cancel (gpio * gpio)
{
  if (gpio->callback == NULL)
    return EXIT_SUCCESS;

  if (gpio->callback->thread == NULL)
  {
    libsoc_gpio_debug (__func__, gpio->gpio, "callback thread was NULL");
    return EXIT_FAILURE;
  }

  pthread_cancel (*gpio->callback->thread);

  pthread_join (*gpio->callback->thread, NULL);

  pthread_mutex_unlock(&gpio->callback->ready);
  pthread_mutex_destroy(&gpio->callback->ready);

  free (gpio->callback->thread);
  free (gpio->callback);

  gpio->callback = NULL;

  libsoc_gpio_debug (__func__, gpio->gpio, "callback thread was stopped");

  return EXIT_SUCCESS;
}
