typedef struct {
  char path[32];
  int fd;
} spi;

int libsoc_spi_set_mode(spi* spi, uint8_t mode);
int libsoc_spi_set_bits_per_word(spi* spi, uint8_t bpw);
spi* libsoc_spi_init (int spidev_device, int chip_select);
int libsoc_spi_write(spi* spi, uint8_t* tx, uint32_t len);
int libsoc_spi_read(spi* spi, uint8_t* rx, uint32_t len);
int libsoc_spi_free(spi* spi);
int libsoc_spi_set_speed(spi* spi, uint32_t speed);
int libsoc_spi_rw(spi* spi, uint8_t* tx, uint8_t* rx, uint32_t len);
