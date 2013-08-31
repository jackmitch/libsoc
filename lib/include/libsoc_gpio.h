/**
 * \struct gpio
 * \brief representation of a single requested gpio
 * \param unsigned int gpio gpio id
 * \param int value_fd file descriptor to gpio value file
 */

typedef struct 
{
  unsigned int gpio;
  int value_fd;
} gpio;

/**
 * \enum gpio_direction
 * \brief defined values for input/output direction
 */

typedef enum
{
  INPUT = 0,
  OUTPUT = 1
} gpio_direction;

/**
 * \enum gpio_level
 * \brief defined values for high/low gpio level
 */

typedef enum
{
  HIGH = 1,
  LOW = 0
} gpio_level;

/**
 * \fn gpio* libsoc_gpio_request(unsigned int gpio_id)
 * \brief request a gpio to use
 * \param unsigned int gpio_id - the id of the gpio you wish to request
 * \return pointer to gpio* on success NULL on fail
 * 
 */

gpio* libsoc_gpio_request(unsigned int gpio_id);

/**
 * \fn int libsoc_gpio_free(gpio* gpio)
 * \brief free a previously requested gpio
 * \param gpio* gpio - valid pointer to a requested gpio
 * \return EXIT_SUCCESS or EXIT_FAILURE 
 */

int libsoc_gpio_free(gpio* gpio);

/**
 * \fn int libsoc_gpio_set_direction(gpio* current_gpio, gpio_direction direction)
 * \brief set GPIO to input or output
 * \param gpio* current_gpio - pointer to gpio struct on which to set the direction
 * \param gpio_direction direction - enumerated direction, INPUT or OUTPUT
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */

int libsoc_gpio_set_direction(gpio* current_gpio, gpio_direction direction);

/**
 * \fn gpio_direction libsoc_gpio_get_direction(gpio* current_gpio)
 * \brief get the current direction of the gpio
 * \param gpio* current_gpio - pointer to gpio struct on which to get the direction
 * \return current GPIO direction, INPUT or OUTPUT
 */

gpio_direction libsoc_gpio_get_direction(gpio* current_gpio);

/**
 * \fn in libsoc_gpio_set_level(gpio* current_gpio, gpio_level level)
 * \brief set the GPIO level to high or low
 * \param gpio* current_gpio - pointer to gpio struct on which to set the level
 * \param gpio_level level = enumerated gpio_level, HIGH or LOW
 * \return EXIT_SUCCES or EXIT_FAILURE
 */

int libsoc_gpio_set_level(gpio* current_gpio, gpio_level level);

/**
 * \fn gpio_level libsoc_gpio_get_level(gpio* current_gpio)
 * \brief gets the current gpio level
 * \param gpio* current_gpio - pointer to gpio struct on which to get the level
 * \return gpio_level, HIGH or LOW
 */

gpio_level libsoc_gpio_get_level(gpio* current_gpio);

/**
 * \fn void libsoc_gpio_set_debug 
 * \brief sets the debug level of the gpio functions, library must have 
 *  been compiled with DEBUG support enabled
 * \param int level - 1 or 0, 1 to enable debug 0 to disable
 */

void libsoc_gpio_set_debug(int level);
