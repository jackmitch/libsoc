/* SPDX-License-Identifier: LGPL-2.1-only */

#ifndef _LIBSOC_CONFFILE_H_
#define _LIBSOC_CONFFILE_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \struct keyval
 * \brief a linked list of key-value pairs
 * \param char *key - the name of the key
 * \param char *val - the value of the key
 * \param keyval* - the next key-value pair
 */

typedef struct keyval {
  char *key;
  char *val;
  struct keyval *next;
} keyval;

/**
 * \struct section
 * \brief Contains the key-value pairs in a given section
 * \param char *name - the section name
 * \param keyval *settings - the key-value pairs for the section
 * \param section* - the next section in the list
 */

typedef struct section {
  char name[16];
  keyval *settings;
  struct section *next;
} section;

/**
 * \struct confile
 * \brief Contains a list of sections
 * \param section* section - the HEAD pointer of the section list
 */
typedef struct conffile {
  section *sections;
} conffile;

/**
 * \fn conffile *conffile_load(const char *path)
 * \brief Loads a given configuration file
 * \param char* path - The path to the file
 * \return NULL on error
 */
conffile* conffile_load(const char *path);

/**
 * \fn conffile *conffile_free(conffile*)
 * \brief Frees memory allocated in conffile struct
 */
void conffile_free(conffile *conf);

/**
 * \fn const char *conffile_get(conffile *conf, const char *sectname, const char *key, const char *defval)
 * \brief Find the key in the given section of conffile and return its value.
 * \return The value found in the conffile or defval if one is not found.
 */
const char* conffile_get(conffile *conf, const char *sectname, const char *key, const char *defval);

/**
 * \fn int *conffile_get_int(conffile *conf, const char *sectname, const char *key, int defval)
 * \brief Find the key in the given section of conffile and return its integer
 * value.
 * \brief Returns the value for the given section and key or defval if the key
 * isn't found or is not numeric.
 */
int conffile_get_int(conffile *conf, const char *sectname, const char *key, int defval);

#ifdef __cplusplus
}
#endif
#endif
