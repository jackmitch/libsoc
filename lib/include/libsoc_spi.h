#ifndef _LIBSOC_SPI_H_
#define _LIBSOC_SPI_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \struct spi
 * \brief representation of spi device and chipselect
 * \param int fd - file descriptor to open spidev character device
 * \param struct gpio_callback *callback - struct used to store interrupt
 *  callback data
 * \param uint8_t spi_dev - major number of spi device
 * \param uint8_t spi_dev - minor number of spi device
 */

typedef struct {
  int fd;
  uint8_t spi_dev;
  uint8_t chip_select;
} spi;

/**
 * \enum gpio_direction
 * \brief defined values for bits per word
 */
typedef enum {
  BITS_8 = 8,
  BITS_16 = 16,
  BPW_ERROR,
} spi_bpw;

/**
 * \enum gpio_direction
 * \brief defined values for spi modes
 */
typedef enum {
  MODE_0 = 0,
  MODE_1 = 1,
  MODE_2 = 2,
  MODE_3 = 3,
  MODE_ERROR,
} spi_mode;

/**
 * \fn spi* libsoc_spi_init (uint8_t spidev_device, uint8_t chip_select)
 * \brief opens the spidev character device and intitialises a new spi
 *  struct
 * \param uint8_t spidev_device - the major spidev number
 * \param uint8_t spidev_device - the minor spidev number
 * \return spi* struct pointer or NULL on failure
 */
spi* libsoc_spi_init (uint8_t spidev_device, uint8_t chip_select);

/**
 * \fn int libsoc_spi_free(spi* spi)
 * \brief frees the malloced spi struct
 * \param spi* spi - valid spi struct pointer
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
int libsoc_spi_free(spi* spi);

/**
 * \fn int libsoc_spi_set_mode(spi* spi, spi_mode mode)
 * \brief sets the spi mode for the spi device
 * \param spi* spi - valid spi struct pointer
 * \param enum spi_mode - valid modes are as follows:
 *   -MODE_0, MODE_1, MODE_2, MODE_3
 *  Essential Reading: 
 *  - https://www.kernel.org/doc/Documentation/spi/spi-summary
 *  - http://www.diolan.com/dln_doc/spi-transfer-modes.html
 *  - http://blackfin.uclinux.org/doku.php?id=spi
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
int libsoc_spi_set_mode(spi* spi, spi_mode mode);

/**
 * \fn int libsoc_spi_set_bits_per_word(spi* spi, spi_bpw bpw)
 * \brief sets the bits per word of the spi transfer, either 8 or 16
 * \param spi* spi - valid spi struct pointer
 * \param enum spi_bpw - bits per word eith BITS_8 or BITS_16
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
int libsoc_spi_set_bits_per_word(spi* spi, spi_bpw bpw);

/**
 * \fn int libsoc_spi_set_speed(spi* spi, uint32_t speed)
 * \brief sets the speed of the spi bus, speed can sometimes be changed
 *  to a sensible value by the kernel, so always do a get and check after
 *  setting the speed.
 * \param spi* spi - valid spi struct pointer
 * \param uint32_t speed - set the spi bus speed in Hz
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
int libsoc_spi_set_speed(spi* spi, uint32_t speed);

/**
 * \fn spi_mode libsoc_spi_get_mode(spi* spi)
 * \brief gets the current mode of the spi bus
 * \param spi* spi - valid spi struct pointer
 * \return enum spi_mode - MODE_0/1/2/3 on success, MODE_ERROR on fail
 */
spi_mode libsoc_spi_get_mode(spi* spi);

/**
 * \fn uint32_t libsoc_spi_get_speed(spi* spi)
 * \brief gets the current speed of the spi bus
 * \param spi* spi - valid spi struct pointer
 * \return uint32 - current speed of spi bus in Hz
 */
uint32_t libsoc_spi_get_speed(spi* spi);

/**
 * \fn spi_bpw libsoc_spi_get_bits_per_word(spi* spi)
 * \brief gets the current bits per word of the spi bus
 * \param spi* spi - valid spi struct pointer
 * \return enum spi_bpw - BITS_8/16 on success, BITS_ERROR on fail
 */
spi_bpw libsoc_spi_get_bits_per_word(spi* spi);

/**
 * \fn int libsoc_spi_write(spi* spi, uint8_t* tx, uint32_t len)
 * \brief writes data to the spi bus
 * \param spi* spi - valid spi struct pointer
 * \param uint8_t* tx - array of bytes to send on the bus
 * \param uint32_t len - the length of the transfer in bytes
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
int libsoc_spi_write(spi* spi, uint8_t* tx, uint32_t len);

/**
 * \fn int libsoc_spi_read(spi* spi, uint8_t* rx, uint32_t len)
 * \brief reads data from the spi bus
 * \param spi* spi - valid spi struct pointer
 * \param uint8_t* rx - array of bytes to populate with data from the bus
 * \param uint32_t len - the length of the transfer in bytes
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
int libsoc_spi_read(spi* spi, uint8_t* rx, uint32_t len);

/**
 * \fn int libsoc_spi_rw(spi* spi, uint8_t* tx, uint8_t* rx, uint32_t len)
 * \brief duplex read/write to the spi bus, allows writing data and then 
 *  reading back in one single transaction, or writing and reading at the
 *  same time.
 * \param spi* spi - valid spi struct pointer
 * \param uint8_t* tx - array of bytes to send on the bus
 * \param uint8_t* rx - array of bytes to populate with data from the bus
 * \param uint32_t len - the length of the transfer in bytes
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
int libsoc_spi_rw(spi* spi, uint8_t* tx, uint8_t* rx, uint32_t len);

#ifdef __cplusplus
}
#endif
#endif
