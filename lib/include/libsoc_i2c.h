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
} i2c;
