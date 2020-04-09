/* SPDX-License-Identifier: LGPL-2.1-only */

#ifndef _LIBSOC_GPIO_ID_H_
#define _LIBSOC_GPIO_ID_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "libsoc_conffile.h"

/**
 * \struct board_config
 * \brief a struct to hold board specific information
 * \param conf* - the board config file
 */

typedef struct {
  conffile *conf;
} board_config;

/**
 * \fn board_config* libsoc_board_init()
 * \brief initialize board specific values like gpio mappings
 * \return board_config* or NULL
 */

board_config *libsoc_board_init();

/**
 * \fn int libsoc_board_free(board_config *config)
 * \brief free up memory created from call to libsoc_board_init
 * \param board_config* config - valid pointer to board_config
 * \return void
 */

void libsoc_board_free(board_config *config);

/**
 * \fn int libsoc_gpio_id(board_config* config, const char* pin)
 * \brief find the gpio id of a given pin name
 * \param board_config* config - valid pointer to board_config
 * \param char* pin - a pin name for the board like "P49"
 * \return >=0 for gpio id or -1 on failure
 */

int libsoc_board_gpio_id(board_config *config, const char* pin);

#ifdef __cplusplus
}
#endif
#endif
