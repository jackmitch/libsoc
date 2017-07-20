#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "libsoc_file.h"
#include "libsoc_debug.h"
#include "libsoc_adc.h"

#define STR_BUF 256

void libsoc_adc_debug (const char *func, unsigned int chip,
  unsigned int adc, char *format, ...)
{
#ifdef DEBUG
  if (libsoc_get_debug ())
  {
    va_list args;

    fprintf (stderr, "libsoc-adc-debug: ");

    va_start (args, format);
    vfprintf (stderr, format, args);
    va_end (args);

    fprintf (stderr, " ((%d,%d), %s)", chip, adc, func);

    fprintf (stderr, "\n");
  }
#endif
}

adc* libsoc_adc_request (unsigned int chip, unsigned int adc_num)
{
  adc *new_adc;
  char chip_str[STR_BUF];
  char adc_str[STR_BUF];
  int i, j;
  size_t result;

  libsoc_adc_debug (__func__, chip, adc_num, "requested ADC");

  for (i = 0; i < sizeof(ADC_DEVICE_FORMAT); i++)
  {
    result = snprintf(chip_str, STR_BUF, ADC_DEVICE_FORMAT[i], chip);
    if (result < 0)
    {
      perror("Failed formatting chip string");
      return NULL;
    }
    if (result >= STR_BUF)
    {
      fprintf(stderr, "Chip format string \"%s\" too long for buffer\n",
              ADC_DEVICE_FORMAT[i]);
      return NULL;
    }
    if (file_valid(chip_str))
    {
      libsoc_adc_debug(__func__, chip, adc_num, "Found requested chip");
      for (j = 0; j < sizeof(ADC_READ_FORMAT); j++)
      {
        strncpy(adc_str, chip_str, STR_BUF);
        strncpy(adc_str + result, ADC_READ_FORMAT[j],
                STR_BUF - strlen(adc_str));
        if (strlen(adc_str) > STR_BUF)
        {
          perror("Failed formatting device string");
          return NULL;
        }
        if (file_valid(adc_str))
        {
          libsoc_adc_debug(__func__, chip, adc_num, "Found requested ADC");
          new_adc = malloc(sizeof(adc));
          new_adc->fd = file_open(adc_str, O_RDONLY);
	}
        if (new_adc->fd < 0)
        {
          free(new_adc);
          libsoc_adc_debug(__func__, chip, adc_num,
                           "Failed to open adc sysfs file: %d", new_adc->fd);
          return NULL;
        }
        new_adc->chip = chip;
        new_adc->adc = adc_num;
        return new_adc;
      }
    }
  }
  libsoc_adc_debug(__func__, chip, adc_num, "Found no such ADC available");
  return NULL;
}

int libsoc_adc_free(adc *adc)
{
  if (adc == NULL)
  {
    libsoc_adc_debug(__func__, -1, -1, "invalid adc pointer");
    return EXIT_FAILURE;
  }

  if (file_close(adc->fd) < 0)
  {
    return EXIT_FAILURE;
  }

  free(adc);
  return EXIT_SUCCESS;
}
/*
 * vim: tabstop=8 expandtab shiftwidth=2 softtabstop=2
 */
