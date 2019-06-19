/* SPDX-License-Identifier: LGPL-2.1-only */

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libsoc_debug.h"
#include "libsoc_conffile.h"

static char *
trim(char *buf)
{
  size_t len = strlen(buf);
  while (isspace(buf[--len]))
    buf[len] = '\0';
  while (isspace(*buf))
    buf++;
  return buf;
}

static keyval*
_create_keyval(const char *path, char *line)
{
  keyval *kv;
  char *key, *val, *save;
  key = strtok_r(line, "=", &save);
  val = strtok_r(NULL, "\n", &save);
  if (!val)
    {
      libsoc_warn("Invalid key = value in %s:\n%s\n", path, line);
      return NULL;
    }
  key = trim(key);
  val = trim(val);
  kv = calloc(1, sizeof(keyval));
  kv->key = strdup(key);
  kv->val = strdup(val);
  return kv;
}

conffile *
conffile_load(const char *path)
{
  int rc;
  FILE *fp;
  char line[256];
  conffile *conf = calloc(1, sizeof(conffile));
  section *stmp = NULL;
  keyval *ktmp = NULL;

  fp = fopen (path, "r");
  if (fp)
    {
      while(fgets(line, sizeof(line), fp))
        {
          if (*line == '#' || *line == '\0' || *line == '\n')
	    continue;
          if (line[0] == '[')
            {
              // new section
              stmp = calloc(1, sizeof(section));
              stmp->next = conf->sections;
              conf->sections = stmp;

              rc = strlen(line);
              if (line[rc-2] != ']' || (rc-2) > sizeof(stmp->name))
                {
                  libsoc_warn("Invalid section line in %s:\n%s\n", path, line);
                  goto cleanup;
                }
              line[rc-2] = '\0';
              strcpy(stmp->name, trim(&line[1]));
            }
          else
            {
              if (!conf->sections)
                {
                  libsoc_warn("Section must be declared in %s before line: %s\n",
                              path, line);
                }
              ktmp = _create_keyval(path, line);
              if (ktmp)
                {
                  ktmp->next = conf->sections->settings;
                  conf->sections->settings = ktmp;
                }
              else
                {
                  goto cleanup;
                }
            }
        }
      fclose(fp);
    }
  else
    {
        libsoc_warn("Unable to open board config: %s\n", path);
    }

  return conf;

cleanup:
  if (fp)
    fclose(fp);
  conffile_free(conf);
  return NULL;
}

void conffile_free(conffile *conf)
{
  section *sptr, *stmp;
  keyval *kptr, *ktmp;

  if (!conf)
    return;

  sptr = conf->sections;
  while (sptr)
    {
      kptr = sptr->settings;
      while (kptr)
        {
          ktmp = kptr->next;
          free(kptr->key);
          free(kptr->val);
          free(kptr);
          kptr = ktmp;
        }
      stmp = sptr->next;
      free(sptr);
      sptr = stmp;
    }
}

const char*
conffile_get(conffile *conf, const char *sectname, const char *key, const char *defval)
{
  section *s = conf->sections;
  while(s)
    {
      if (!strcmp(s->name, sectname))
        {
          keyval *kv = s->settings;
          while(kv)
            {
              if (!strcmp(kv->key, key))
                return kv->val;
              kv = kv->next;
            }
          return defval;
        }
      s = s->next;
    }
  return defval;
}


int
conffile_get_int(conffile *conf, const char *sectname, const char *key, int defval)
{
  long val = defval;
  const char *strval = conffile_get(conf, sectname, key, NULL);
  if (strval)
    {
      char *endptr;
      errno = 0;
      val = strtol(strval, &endptr, 10);
      if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
            || (errno != 0 && val == 0) || endptr == strval || *endptr != '\0'
            || val > INT_MAX)
        {
          libsoc_warn("Invalid number: %s\n", strval);
          val = defval;
        }
    }
  return val;
}
