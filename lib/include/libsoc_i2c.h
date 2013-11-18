#include <linux/i2c.h>
#include <linux/i2c-dev.h>

/**
 * \struct i2c
 * \brief representation of spi device and chipselect
 * \param int fd - file descriptor to open i2c device
 * \param uint8_t bus - i2c bus number
 * \param uint8_t address - address of i2c device on the bus
 */

typedef struct {
  int fd;
  uint8_t bus;
  uint8_t address;
  struct i2c_rdwr_ioctl_data packets;
  struct i2c_msg messages[2];
} i2c;

/**
 * \fn i2c * libsoc_i2c_init (uint8_t i2c_bus, uint8_t i2c_address)
 * \brief initialises new i2c instance at specified address
 * \param uint8_t i2c_bus - the linux enumerated bus number
 * \param uint8_t i2c_address - 
 * \return i2c* struct pointer or NULL on failure
 */
i2c * libsoc_i2c_init (uint8_t i2c_bus, uint8_t i2c_address);

/**
 * \fn libsoc_i2c_free (i2c * i2c)
 * \brief frees the malloced i2c struct
 * \param i2c* i2c - valid i2c struct pointer
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
int libsoc_i2c_free (i2c * i2c);

int libsoc_i2c_write (i2c * i2c, uint8_t * buffer, uint16_t len);
int libsoc_i2c_read (i2c * i2c, uint8_t * buffer, uint16_t len);
int libsoc_i2c_set_timeout(i2c * i2c, int timeout);
