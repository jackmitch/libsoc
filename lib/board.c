/* SPDX-License-Identifier: LGPL-2.1-only */

#include <dirent.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "libsoc_board.h"
#include "libsoc_debug.h"
#include "libsoc_file.h"

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

static board_config *
_probe()
{
  board_config *bc = NULL;
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
                          bc = calloc(1, sizeof(board_config));
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
      bc = _probe();
      if (!bc)
        libsoc_warn("Board config(%s) does not exist and could not be probed", conf);
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

int
libsoc_board_gpio_id(board_config *config, const char* pin)
{
  return conffile_get_int(config->conf, "GPIO", pin, -1);
}
