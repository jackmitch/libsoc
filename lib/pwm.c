#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "libsoc_file.h"
#include "libsoc_debug.h"
#include "libsoc_pwm.h"

#define STR_BUF 256

static char pwm_polarity_strings[2][STR_BUF] = { "normal", "inversed" };
static char pwm_enabled_strings[2][STR_BUF] = { "0", "1" };

inline void libsoc_pwm_debug (const char *func, unsigned int chip,
  unsigned int pwm, char *format, ...)
{
#ifdef DEBUG
  if (libsoc_get_debug ())
  {
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
  char tmp_str[STR_BUF];
  int shared = 0;

  if (mode != LS_SHARED && mode != LS_GREEDY && mode != LS_WEAK)
  {
    libsoc_pwm_debug (__func__, chip, pwm_num,
	    "mode was not set, or invalid, setting mode to LS_SHARED");

    mode = LS_SHARED;
  }

  libsoc_pwm_debug (__func__, chip, pwm_num, "requested PWM");

  sprintf (tmp_str, "/sys/class/pwm/pwmchip%d/pwm%d/enable", chip, pwm_num);

  if (file_valid (tmp_str))
  {
    libsoc_pwm_debug(__func__, chip, pwm_num, "PWM already exported");

    switch(mode)
	  {
	    case LS_WEAK:
	    {
	      return NULL;
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

    if (file_write_int_path(tmp_str, pwm_num) == EXIT_FAILURE)
    {
      libsoc_pwm_debug(__func__, chip, pwm_num, "write failed");
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
  new_pwm->enable_fd = file_open(tmp_str, O_SYNC | O_RDWR);

  sprintf(tmp_str, "/sys/class/pwm/pwmchip%d/pwm%d/period", chip, pwm_num);
  new_pwm->period_fd = file_open(tmp_str, O_SYNC | O_RDWR);

  sprintf(tmp_str, "/sys/class/pwm/pwmchip%d/pwm%d/duty_cycle", chip, pwm_num);
  new_pwm->duty_fd = file_open(tmp_str, O_SYNC | O_RDWR);

  if (new_pwm->enable_fd < 0 || new_pwm->period_fd < 0 || new_pwm->duty_fd < 0)
  {
    free(new_pwm);
	  libsoc_pwm_debug(__func__, chip, pwm_num, "Failed to open pwm sysfs file: %d", new_pwm->enable_fd);
    return NULL;
  }

  new_pwm->chip = chip;
  new_pwm->pwm = pwm_num;
  new_pwm->shared = shared;

  return new_pwm;
}

int libsoc_pwm_free(pwm *pwm)
{
  char path[STR_BUF];

  if (pwm == NULL)
  {
    libsoc_pwm_debug(__func__, -1, -1, "invalid pwm pointer");
    return EXIT_FAILURE;
  }

  libsoc_pwm_debug(__func__, pwm->chip, pwm->pwm, "freeing pwm");

  if (file_close(pwm->enable_fd) < 0)
  {
    return EXIT_FAILURE;
  }

  if (file_close(pwm->period_fd) < 0)
  {
    return EXIT_FAILURE;
  }

  if (file_close(pwm->duty_fd) < 0)
  {
    return EXIT_FAILURE;
  }

  if (pwm->shared == 1)
  {
    free(pwm);
    return EXIT_SUCCESS;
  }

  sprintf(path, "/sys/class/pwm/pwmchip%d/unexport", pwm->chip);

  file_write_int_path(path, pwm->pwm);

  sprintf(path, "/sys/class/pwm/pwmchip%d/pwm%d", pwm->chip, pwm->pwm);

  if (file_valid(path))
  {
    libsoc_pwm_debug(__func__, pwm->chip, pwm->pwm, "freeing failed");
    return EXIT_FAILURE;
  }

  free(pwm);

  return EXIT_SUCCESS;
}

int libsoc_pwm_set_enabled(pwm *pwm, pwm_enabled enabled)
{
  char path[STR_BUF];

  if (pwm == NULL)
  {
    libsoc_pwm_debug(__func__, -1, -1, "invalid pwm pointer");
    return EXIT_FAILURE;
  }

  if (enabled != ENABLED && enabled != DISABLED)
  {
    return EXIT_FAILURE;
  }

  libsoc_pwm_debug(__func__, pwm->chip, pwm->pwm,
    "setting enabled to %s", pwm_enabled_strings[enabled]);

  sprintf(path, "/sys/class/pwm/pwmchip%d/pwm%d/enable", pwm->chip, pwm->pwm);

  return file_write_str(path, pwm_enabled_strings[enabled], 1);
}

pwm_enabled libsoc_pwm_get_enabled(pwm *pwm)
{
  int val;

  if (pwm == NULL)
  {
    libsoc_pwm_debug(__func__, -1, -1, "invalid pwm pointer");
    return ENABLED_ERROR;
  }

  if (file_read_int_fd(pwm->enable_fd, &val) == EXIT_FAILURE)
  {
    return ENABLED_ERROR;
  }

  if(val == 1)
  {
    libsoc_pwm_debug(__func__, pwm->chip, pwm->pwm,
		  "read as enabled");
    return ENABLED;
  }
  else if (val == 0)
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

int libsoc_pwm_set_period(pwm *pwm, unsigned int period)
{
  if (pwm == NULL)
  {
    libsoc_pwm_debug(__func__, -1, -1, "invalid pwm pointer");
    return EXIT_FAILURE;
  }

  libsoc_pwm_debug(__func__, pwm->chip, pwm->pwm,
    "setting period to %d", period);

  return file_write_int_fd(pwm->period_fd, period);
}

int libsoc_pwm_set_duty_cycle(pwm *pwm, unsigned int duty)
{
  if (pwm == NULL)
  {
    libsoc_pwm_debug(__func__, -1, -1, "invalid pwm pointer");
    return EXIT_FAILURE;
  }

  libsoc_pwm_debug(__func__, pwm->chip, pwm->pwm,
    "setting duty to %d", duty);

  return file_write_int_fd(pwm->duty_fd, duty);
}

int libsoc_pwm_get_period(pwm *pwm)
{
  int period = -1;

  if (pwm == NULL)
  {
    libsoc_pwm_debug(__func__, -1, -1, "invalid pwm pointer");
    return -1;
  }

  file_read_int_fd(pwm->period_fd, &period);

  libsoc_pwm_debug(__func__, pwm->chip, pwm->pwm, "got period as %d", period);

  return period;
}

int libsoc_pwm_get_duty_cycle(pwm *pwm)
{
  int duty = -1;

  if (pwm == NULL)
  {
    libsoc_pwm_debug(__func__, -1, -1, "invalid pwm pointer");
    return -1;
  }

  file_read_int_fd(pwm->duty_fd, &duty);

  libsoc_pwm_debug(__func__, pwm->chip, pwm->pwm, "got duty as %d", duty);

  return duty;
}

int libsoc_pwm_set_polarity(pwm *pwm, pwm_polarity polarity)
{
  char path[STR_BUF];

  if (pwm == NULL)
  {
    libsoc_pwm_debug(__func__, -1, -1, "invalid pwm pointer");
    return EXIT_FAILURE;
  }

  if (polarity != NORMAL && polarity != INVERSED)
  {
    return EXIT_FAILURE;
  }

  libsoc_pwm_debug(__func__, pwm->chip, pwm->pwm,
    "setting polarity to %s", pwm_polarity_strings[polarity]);

  sprintf(path, "/sys/class/pwm/pwmchip%d/pwm%d/polarity", pwm->chip, pwm->pwm);

  return file_write_str(path, pwm_polarity_strings[polarity], STR_BUF);
}

int libsoc_pwm_get_polarity(pwm *pwm)
{
  int polarity;
  char path[STR_BUF];
  char tmp_str[1];

  if (pwm == NULL)
  {
    libsoc_pwm_debug(__func__, -1, -1, "invalid pwm pointer");
    return EXIT_FAILURE;
  }

  sprintf(path, "/sys/class/pwm/pwmchip%d/pwm%d/polarity", pwm->chip, pwm->pwm);

  if (file_read_str(path, tmp_str, 1) == EXIT_FAILURE)
  {
    return POLARITY_ERROR;
  }

  if (strncmp(tmp_str, "i", 1) == 0)
  {
    polarity = INVERSED;
  }
  else if (strncmp(tmp_str, "n", 1) == 0)
  {
    polarity = NORMAL;
  }
  else
  {
    polarity = POLARITY_ERROR;
  }

  if (polarity > 0)
  {
    libsoc_pwm_debug(__func__, pwm->chip, pwm->pwm, "got polarity as %s", pwm_polarity_strings[polarity]);
  }
  else
  {
    libsoc_pwm_debug(__func__, pwm->chip, pwm->pwm, "getting polarity failed");
  }

  return polarity;
}

