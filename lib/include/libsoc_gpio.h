#ifndef _LIBSOC_GPIO_H_
#define _LIBSOC_GPIO_H_

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \struct gpio_callback
 * \brief representation of an interrupt callback
 * \param int (*callback_fn)(void*) - the function to callback on interrupt
 * \param void *callback_arg - the argument to pass to the callback function
 * \param pthread_t *thread - the pthread struct on which the poll and
 *  callback function runs
 * \param int ready - signal when the pthread is ready to accept interrupts
 */

struct gpio_callback {
	int (*callback_fn) (void *);
	void *callback_arg;
	pthread_t *thread;
	int ready;
};

/**
 * \struct gpio
 * \brief representation of a single requested gpio
 * \param unsigned int gpio gpio id
 * \param int value_fd file descriptor to gpio value file
 * \param struct gpio_callback *callback - struct used to store interrupt
 *  callback data
 * \param int shared - set if the request flag was shared and the GPIO was
 *  exported on request
 */

typedef struct {
	unsigned int gpio;
	int value_fd;
	struct gpio_callback *callback;
	int shared;
} gpio;

/**
 * \enum gpio_direction
 * \brief defined values for input/output direction
 */

typedef enum {
	DIRECTION_ERROR = -1,
	INPUT = 0,
	OUTPUT = 1,
} gpio_direction;

/**
 * \enum gpio_level
 * \brief defined values for high/low gpio level
 */

typedef enum {
	LEVEL_ERROR = -1,
	LOW = 0,
	HIGH = 1,
} gpio_level;

/**
 * \enum gpio_edge
 * \brief defined values for rising/falling/none/both gpio edge
 */

typedef enum {
	EDGE_ERROR = -1,
	RISING = 0,
	FALLING = 1,
	NONE = 2,
	BOTH = 3,
} gpio_edge;

/**
 * \enum gpio_mode  
 * 
 * LS_SHARED - if the gpio is already exported then it will not unexport
 *             the GPIO on free. If it is not exported, then it will
 *             unexport on free.
 * 
 * LS_GREEDY - will succeed if the GPIO is already exported, but will 
 *             always unexport the GPIO on free.
 * 
 * LS_WEAK   - will fail if GPIO is already exported, will always unexport
 *             on free.
 */

enum gpio_mode {
	LS_SHARED,
	LS_GREEDY,
	LS_WEAK,
};

/**
 * \fn gpio* libsoc_gpio_request(unsigned int gpio_id)
 * \brief request a gpio to use
 * \param unsigned int gpio_id - the id of the gpio you wish to request
 * \param unsigned int mode - mode for opening GPIO
 * \return pointer to gpio* on success NULL on fail
 * 
 */

gpio *libsoc_gpio_request(unsigned int gpio_id, enum gpio_mode mode);

/**
 * \fn int libsoc_gpio_free(gpio* gpio)
 * \brief free a previously requested gpio
 * \param gpio* gpio - valid pointer to a requested gpio
 * \return EXIT_SUCCESS or EXIT_FAILURE 
 */

int libsoc_gpio_free(gpio * gpio);

/**
 * \fn int libsoc_gpio_set_direction(gpio* current_gpio, gpio_direction direction)
 * \brief set GPIO to input or output
 * \param gpio* current_gpio - pointer to gpio struct on which to set the direction
 * \param gpio_direction direction - enumerated direction, INPUT or OUTPUT
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */

int libsoc_gpio_set_direction(gpio * current_gpio, gpio_direction direction);

/**
 * \fn gpio_direction libsoc_gpio_get_direction(gpio* current_gpio)
 * \brief get the current direction of the gpio
 * \param gpio* current_gpio - pointer to gpio struct on which to get the direction
 * \return current GPIO direction, INPUT or OUTPUT
 */

gpio_direction libsoc_gpio_get_direction(gpio * current_gpio);

/**
 * \fn in libsoc_gpio_set_level(gpio* current_gpio, gpio_level level)
 * \brief set the GPIO level to high or low
 * \param gpio* current_gpio - pointer to gpio struct on which to set the level
 * \param gpio_level level = enumerated gpio_level, HIGH or LOW
 * \return EXIT_SUCCES or EXIT_FAILURE
 */

int libsoc_gpio_set_level(gpio * current_gpio, gpio_level level);

/**
 * \fn gpio_level libsoc_gpio_get_level(gpio* current_gpio)
 * \brief gets the current gpio level
 * \param gpio* current_gpio - pointer to gpio struct on which to get the level
 * \return gpio_level, HIGH or LOW
 */

gpio_level libsoc_gpio_get_level(gpio * current_gpio);

/**
 * \fn void libsoc_gpio_set_debug 
 * \brief sets the debug level of the gpio functions, library must have 
 *  been compiled with DEBUG support enabled
 * \param int level - 1 or 0, 1 to enable debug 0 to disable
 */

void libsoc_gpio_set_debug(int level);

/**
 * \fn gpio_edge libsoc_gpio_get_edge(gpio* current_gpio)
 * \brief gets the current gpio edge value
 * \param gpio* current_gpio - pointer to gpio struct on which to get the edge
 * \return gpio_edge, RISING, FALLING or NONE
 */

gpio_edge libsoc_gpio_get_edge(gpio * current_gpio);

/**
 * \fn int libsoc_gpio_set_edge(gpio* current_gpio, gpio_edge edge)
 * \brief sets the edge value of a gpio for interrupt generation
 * \param gpio* current_gpio - the gpio for which you wish to set the edge
 * \param gpio_edge edge - enumerated edge value, RISING, FALLING or NONE
 * \return EXIT_SUCCESS or EXIT FAILURE
 */

int libsoc_gpio_set_edge(gpio * current_gpio, gpio_edge edge);

/**
 * \fn int libsoc_gpio_wait_interrupt(gpio* gpio, int timeout)
 * \brief takes a gpio and waits for length of timeout or until an
 *  interrupt occurs. Will return EXIT_SUCCESS on interrupt or 
 *  EXIT_FAILURE on error or timeout.
 * \param gpio*  gpio - the gpio for which you want to wait on interrupt
 * \param int timeout - the max length of time to wait for interrupt in 
 *  milliseconds
 * \return EXIT_SUCCESS on interrupt caught, EXIT_FAILURE on error or 
 *  interrupt missed
 */

int libsoc_gpio_wait_interrupt(gpio * gpio, int timeout);

/**
 * \fn int libsoc_gpio_callback_interrupt(gpio* gpio, int (*callback_fn)(void*), void* arg)
 * \brief takes a gpio and a callback function, when an interrupt occurs
 *  on the edge previously specified, the callback function is called
 * \param gpio* gpio - the gpio for which you want the interrupt to 
 *  trigger the callback function
 * \param int (*callback_fn)(void*) - the function you wish to call with
 *  a void* pointer to a possible value you may wish to use in the callback.
 * \param void* arg - pointer to data you wish to use in the callback function
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */

int libsoc_gpio_callback_interrupt(gpio * gpio, int (*callback_fn) (void *),
				   void *arg);

/**
 * \fn int libsoc_gpio_callback_interrupt_cancel(gpio* gpio)
 * \brief cancel a callback on a gpio interrupt
 * \param gpio* gpio - gpio with a valid callback enabled
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */

int libsoc_gpio_callback_interrupt_cancel(gpio * gpio);

#ifdef __cplusplus
}
#endif
#endif
