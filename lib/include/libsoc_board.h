#ifndef _LIBSOC_GPIO_ID_H_
#define _LIBSOC_GPIO_ID_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \struct pin_mapping
 * \brief a linked list of pin-name to gpio id mappings for a given board
 * \param char[16] - the pin name used by the board, ie "P49"
 * \param unsigned int - the gpio id of the given pin
 * \param pin_mapping* - the next mapping in the list for the board
 */

typedef struct pin_mapping {
  char pin[16];
  unsigned int gpio;
  struct pin_mapping *next;
} pin_mapping;

/**
 * \struct board_config
 * \brief a struct to hold board specific information
 * \param pin_mappings* - the head of the linked list of pin-mappings
 */

typedef struct {
  pin_mapping *pin_mappings;
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
 * \fn unsigned int libsoc_gpio_id(board_config* config, const char* pin)
 * \brief find the gpio id of a given pin name
 * \param board_config* config - valid pointer to board_config
 * \param char* pin - a pin name for the board like "P49"
 * \return >=0 for gpio id or -1 on failure
 */

unsigned int libsoc_board_gpio_id(board_config *config, const char* pin);

#ifdef __cplusplus
}
#endif
#endif
