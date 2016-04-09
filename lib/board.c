#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

#include "libsoc_board.h"
#include "libsoc_debug.h"

#define HIKEY DATA_DIR"/hikey/libsoc_gpio.conf"
#define DRAGONBOARD410C DATA_DIR"/dragonboard410c/libsoc_gpio.conf"
#define BUBBLEGUM DATA_DIR"/bubblegum/libsoc_gpio.conf"
#define BEAGLEBONE DATA_DIR"/beaglebone/libsoc_gpio.conf"

static const char *
_probe_cpuinfo()
{
  FILE *fp;
  char line[128];
  const char *name = NULL;

  fp = fopen("/proc/cpuinfo", "r");
  if (fp)
    {
      while(fgets(line, sizeof(line), fp))
        {
          if (strncmp(line, "Hardware", 8) == 0)
            {
              if (strstr(line, "Generic AM33XX"))
                  name = BEAGLEBONE;
            }
        }
      fclose(fp);
    }

  return name;
}

static const char *
_probe_dt()
{
  FILE *fp;
  char buff[64];
  const char *name = NULL;

  fp = fopen("/proc/device-tree/model", "r");
  if (fp)
    {
      if (fgets(buff, sizeof(buff), fp))
        {
          if(!strcmp(buff, "HiKey Development Board"))
            name = HIKEY;
          else if(!strcmp(buff, "Qualcomm Technologies, Inc. APQ 8016 SBC"))
            name = DRAGONBOARD410C;
          else if(!strcmp(buff, "s900"))
            name = BUBBLEGUM;
        }
      fclose(fp);
    }

  return name;
}

static const char *
_probe_for_config()
{
  const char *name = _probe_dt();
  if (!name)
    name = _probe_cpuinfo();
  return name;
}

static const char *
_get_conf_file()
{
  const char *name = getenv("LIBSOC_GPIO_CONF");
  if (name == NULL)
    {
      name = GPIO_CONF;
      if (access(name, F_OK))
        {
          const char *probed = _probe_for_config();
          if (!probed)
            {
              libsoc_warn("GPIO mapping file(%s) does not exist\n", name);
              return NULL;
            }
          name = probed;
        }
    }
  return name;
}

static void
rtrim(char *buff)
{
  size_t len = strlen(buff);
  while (len-- > 0)
    {
      if (isspace(buff[len]))
        buff[len] = '\0';
    }
}

board_config*
libsoc_board_init()
{
  int rc;
  FILE *fp;
  char line[256];
  board_config *bc;
  pin_mapping *cur = NULL;
  pin_mapping *ptr;
  char *tmp;
  const char *conf = _get_conf_file();

  if (!conf)
    return NULL;

  bc = calloc(sizeof(board_config), 1);

  fp = fopen(conf, "r");
  if (fp)
    {
      while(fgets(line, sizeof(line), fp))
        {
          if (*line == '#' || *line == '\0' || *line == '\n') continue;
          ptr = calloc(sizeof(pin_mapping), 1);
          rc = sscanf(line, "%15[^=]=%d", ptr->pin, &ptr->gpio);
          if (rc != 2)
            {
              libsoc_warn("Invalid mapping line in %s:\n%s\n", conf, line);
              goto fail_close;
            }
          rtrim(ptr->pin);

          if (!cur)
            {
              bc->pin_mappings = cur = ptr;
            }
          else
            {
              cur->next = ptr;
              cur = cur->next;
            }
        }
    }
  else
    {
      libsoc_warn("Unable to read pin mapping file: %s\n", conf);
      goto fail;
    }
  return bc;

fail_close:
  fclose(fp);
fail:
  free(bc);
  return NULL;
}

void
libsoc_board_free(board_config *config)
{
  pin_mapping *ptr;
  if (config)
    {
        while(config->pin_mappings)
          {
            ptr = config->pin_mappings->next;
            free(config->pin_mappings);
            config->pin_mappings = ptr;
          }
        free(config);
    }
}

unsigned int
libsoc_board_gpio_id(board_config *config, const char* pin)
{
  pin_mapping *ptr = NULL;
  if (!config)
    return -1;

  ptr = config->pin_mappings;
  while(ptr)
    {
      if (!strcmp(pin, ptr->pin))
        return ptr->gpio;
      ptr = ptr->next;
    }
  return -1;
}
