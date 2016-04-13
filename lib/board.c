#include <stdlib.h>
#include <unistd.h>

#include "libsoc_board.h"
#include "libsoc_debug.h"
#include "libsoc_file.h"

static const char *
_get_conf_file()
{
  const char *name = getenv("LIBSOC_GPIO_CONF");
  if (name == NULL)
    name = GPIO_CONF;
  return name;
}

board_config*
libsoc_board_init()
{
  board_config *bc = NULL;
  const char *conf = _get_conf_file();

  if (!access(conf, F_OK))
    {
      bc = calloc(sizeof(board_config), 1);
      bc->conf = conffile_load(conf);
      if (!bc->conf)
        {
          free(bc);
          bc = NULL;
        }
    }
    else
    {
      libsoc_warn("Board config(%s) does not exist\n", conf);
    }

  return bc;
}

void
libsoc_board_free(board_config *config)
{
  if (config)
    {
        conffile_free(config->conf);
        free(config);
    }
}

unsigned int
libsoc_board_gpio_id(board_config *config, const char* pin)
{
  return conffile_get_int(config->conf, "GPIO", pin, -1);
}
