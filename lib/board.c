#include <dirent.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "libsoc_board.h"
#include "libsoc_debug.h"
#include "libsoc_file.h"
#include "libsoc_conffile.h"

/*
 * The actual board_config type, to hold board specific information
 * magic - A 32-bit magic constant to check validity of opaque pointer
 * conf* - the board config file
 */

typedef struct {
  uint32_t  magic;
  conffile *conf;
} board_config_imp;


// Random (and probably) unique value
#define MAGIC 0x6abc7940


static const char *
_get_conf_file()
{
  const char *name = getenv("LIBSOC_CONF");
  if (name == NULL)
    name = LIBSOC_CONF;
  return name;
}

static int
_probe_config(conffile *conf)
{
  const char *dtfile = conffile_get(conf, "board", "dtfile", "/proc/device-tree/model");
  const char *match = conffile_get(conf, "board", "model", NULL);
  if (match)
    {
      char *probed = file_read_contents(dtfile);
      if (probed)
        {
          int rc = strcmp(probed, match);
	  free(probed);
	  return !rc;
        }
    }
  else
    {
      libsoc_debug(__func__, "No 'model' value found in 'board' section");
    }
  return 0;
}

static board_config_imp *
_probe()
{
  board_config_imp *bc = NULL;
  conffile *conf = NULL;
  const char *confs_dir = DATA_DIR;
  char tmp[PATH_MAX];
  DIR *dirp;
  struct dirent *dp;

  if ((dirp = opendir(confs_dir)) != NULL)
    {
      do
        {
          if ((dp = readdir(dirp)) != NULL)
            {
              char *ext = strrchr(dp->d_name, '.');
              if (ext && !strcmp(ext, ".conf"))
                {
		  strcpy(tmp, confs_dir);
		  strcat(tmp, "/");
		  strcat(tmp, dp->d_name);
		  conf = conffile_load(tmp);
		  if (conf)
		    {
		      libsoc_debug(__func__, "probing %s for board support", tmp);
                      if(_probe_config(conf))
		        {
			  libsoc_debug(__func__, "probing match for %s", tmp);
                          bc = calloc(1, sizeof(board_config_imp));
                          bc->conf = conf;
			}
		      else
		        conffile_free(conf);
		    }
                }
            }
        } while (dp != NULL && bc == NULL);
      closedir(dirp);
    }
  else
    {
      libsoc_warn("Unable to read directory: %s", confs_dir);
    }
  return bc;
}

board_config*
libsoc_board_init()
{
  board_config_imp *bc = NULL;
  const char *conf = _get_conf_file();

  if (!access(conf, F_OK))
    {
      bc = calloc(sizeof(board_config_imp), 1);
      bc->conf = conffile_load(conf);
      if (!bc->conf)
        {
          free(bc);
          bc = NULL;
        }
    }
    else
    {
      bc = _probe();
      if (!bc)
        libsoc_warn("Board config(%s) does not exist and could not be probed", conf);
    }

  if (bc != NULL) bc->magic = MAGIC;
  
  return (board_config*) bc;
}

void
libsoc_board_free(board_config *cfg)
{
  board_config_imp *config = (board_config_imp*) cfg;

  if (!config || config->magic != MAGIC) {
    libsoc_error(__func__, "Invalid board configuration.");
    return;
  }

  conffile_free(config->conf);
  free(config);
}

int
libsoc_board_gpio_id(board_config *cfg, const char* pin)
{
  board_config_imp *config = (board_config_imp*) cfg;

  if (!config || config->magic != MAGIC) {
    libsoc_error(__func__, "Invalid board configuration.");
    return -1;
  }

  return conffile_get_int(config->conf, "GPIO", pin, -1);
}
