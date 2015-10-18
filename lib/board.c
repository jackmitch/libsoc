#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "libsoc_board.h"
#include "libsoc_debug.h"

static const char *
_get_conf_file()
{
  const char *name = getenv("LIBSOC_GPIO_CONF");
  if (name == NULL)
    name = GPIO_CONF;
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
              return NULL;
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
      return NULL;
    }
  return bc;
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
