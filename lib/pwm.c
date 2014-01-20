#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <poll.h>
#include <pthread.h>
#include <errno.h>

#include "libsoc_file.h"
#include "libsoc_debug.h"
#include "libsoc_pwm.h"

#define STR_BUF 256

const char pwm_polarity_strings[2][STR_BUF] = { "normal", "inverted" };
const char pwm_enabled_strings[2][STR_BUF] = { "0", "1" };

inline void
libsoc_pwm_debug (const char *func, unsigned int chip, unsigned int pwm,
  char *format, ...)
{
#ifdef DEBUG
  if (libsoc_get_debug ()) {

    va_list args;

    fprintf (stderr, "libsoc-pwm-debug: ");

    va_start (args, format);
    vfprintf (stderr, format, args);
    va_end (args);

	  fprintf (stderr, " ((%d,%d), %s)", chip, pwm, func);

    fprintf (stderr, "\n");
  }
#endif
}

pwm* libsoc_pwm_request (unsigned int chip, unsigned int pwm_num,
  enum shared_mode mode)
{
  pwm *new_pwm;
  int fd, ret;
  char tmp_str[STR_BUF];
  int shared = 0;

  if (mode != LS_SHARED && mode != LS_GREEDY && mode != LS_WEAK)
  {
    libsoc_pwm_debug (__func__, chip, pwm_num,
	    "mode was not set, or invalid, setting mode to LS_SHARED");

    mode = LS_SHARED;
  }

  libsoc_pwm_debug (__func__, chip, pwm_num, "requested PWM");

  sprintf (tmp_str, "/sys/class/new_pwm/new_pwmchip%d/new_pwm%d/enable", chip, pwm_num);

  if (file_valid (tmp_str))
  {
    libsoc_pwm_debug(__func__, chip, pwm_num, "PWM already exported");

    switch(mode)
	  {
	    case LS_WEAK:
	    {
	      return NULL;
	      break;
	    }

    	case LS_SHARED:
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
    sprintf(tmp_str, "/sys/class/pwm/pwmchip%d/export", chip);

    fd = file_open(tmp_str, O_SYNC | O_WRONLY);

    if (fd < 0)
    {
	    return NULL;
    }

    sprintf(tmp_str, "%d", pwm_num);

    if(file_write(fd, tmp_str, STR_BUF) < 0)
    {
	    return NULL;
    }

    if(file_close(fd))
    {
	    return NULL;
    }

    sprintf(tmp_str, "/sys/class/pwm/pwmchip%d/pwm%d/enable", chip, pwm_num);

    if (!file_valid(tmp_str))
	  {
	    libsoc_pwm_debug(__func__, chip, pwm_num,
			  "failed to export PWM");

      perror("libsoc-pwm-debug");
	    return NULL;
	  }
  }

  new_pwm = malloc(sizeof(pwm));

  sprintf(tmp_str, "/sys/class/pwm/pwmchip%d/pwm%d/enable", chip, pwm_num);

  if (new_pwm->enabled_fd  < 0)
  {
    free(new_pwm);
    return NULL;
  }

  new_pwm->chip = chip;
  new_pwm->pwm = pwm_num;
  new_pwm->shared = shared;

  return new_pwm;
}

int libsoc_pwm_free(pwm *pwm)
{
  char tmp_str[STR_BUF];
  int fd;

  if (pwm == NULL)
  {
    libsoc_pwm_debug(__func__, -1, -1, "invalid pwm pointer");
    return EXIT_FAILURE;
  }

  libsoc_pwm_debug(__func__, pwm->chip, pwm->pwm, "freeing pwm");

  if (file_close(pwm->enabled_fd) < 0)
    return EXIT_FAILURE;

  if (pwm->shared == 1)
  {
    free(pwm);
    return EXIT_SUCCESS;
  }

  sprintf (tmp_str, "/sys/class/pwm/pwmchip%d/unexport", pwm->chip);

  fd = file_open(tmp_str, O_SYNC | O_WRONLY);

  if(fd < 0)
    return EXIT_FAILURE;

  sprintf(tmp_str, "%d", pwm->pwm);

  if(file_write(fd, tmp_str, STR_BUF) < 0)
    return EXIT_FAILURE;

  if(file_close (fd) < 0)
    return EXIT_FAILURE;

  sprintf (tmp_str, "/sys/class/pwm/pwmchip%d/pwm%d", pwm->chip, pwm->pwm);

  if (file_valid (tmp_str))
  {
    libsoc_pwm_debug (__func__, pwm->chip, pwm->pwm, "freeing failed");
    return EXIT_FAILURE;
  }

  free (pwm);

  return EXIT_SUCCESS;
}

int
libsoc_pwm_set_enabled(pwm *pwm, pwm_enabled enabled)
{
  int fd;
  char path[STR_BUF];

  if (pwm == NULL)
  {
    libsoc_pwm_debug(__func__, -1, -1, "invalid pwm pointer");
    return EXIT_FAILURE;
  }

  libsoc_pwm_debug(__func__, pwm->chip, pwm->pwm,
    "setting enabled to %s", pwm_enabled_strings[enabled]);

  sprintf(path, "/sys/class/pwm/pwmchip%d/pwm%d/enable", pwm->chip, pwm->pwm);

  fd = file_open(path, O_SYNC | O_WRONLY);

  if (fd < 0)
    return EXIT_FAILURE;

  if (file_write(fd, pwm_enabled_strings[enabled], STR_BUF) < 0)
    return EXIT_FAILURE;

  if (file_close(fd) < 0)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}

pwm_enabled libsoc_pwm_get_enabled(pwm *pwm)
{
  int fd;
  char tmp_str[STR_BUF];

  if (pwm == NULL)
  {
    libsoc_pwm_debug(__func__, -1, -1, "invalid pwm pointer");
    return ENABLED_ERROR;
  }

  sprintf(tmp_str, "/sys/class/pwm/pwmchip%d/pwm%d/enable", pwm->chip, pwm->pwm);

  fd = file_open(tmp_str, O_RDONLY);

  if (fd < 0)
    return ENABLED_ERROR;

  lseek(fd, 0, SEEK_SET);

  if(file_read (fd, tmp_str, STR_BUF) < 0)
    return ENABLED_ERROR;

  if(file_close (fd) < 0)
    return ENABLED_ERROR;

  if(strncmp(tmp_str, pwm_enabled_strings[ENABLED], 1) == 0)
  {
    libsoc_pwm_debug(__func__, pwm->chip, pwm->pwm,
		  "read as enabled");
    return ENABLED;
  }
  else if (strncmp(tmp_str, pwm_enabled_strings[DISABLED], 1) == 0)
  {
    libsoc_pwm_debug(__func__, pwm->chip, pwm->pwm,
      "read as disabled");
    return DISABLED;
  }
  else
  {
    return ENABLED_ERROR;
  }
}
