typedef struct {
  int fd;
  uint8_t spi_dev;
  uint8_t chip_select;
} spi;

typedef enum {
  BITS_8 = 8,
  BITS_16 = 16,
  BPW_ERROR,
} spi_bpw;

typedef enum {
  MODE_0 = 0,
  MODE_1 = 1,
  MODE_2 = 2,
  MODE_3 = 3,
  MODE_ERROR,
} spi_mode;

spi* libsoc_spi_init (uint8_t spidev_device, uint8_t chip_select);

int libsoc_spi_free(spi* spi);

int libsoc_spi_set_mode(spi* spi, spi_mode mode);
int libsoc_spi_set_bits_per_word(spi* spi, spi_bpw bpw);
int libsoc_spi_set_speed(spi* spi, uint32_t speed);

spi_mode libsoc_spi_get_mode(spi* spi);
uint32_t libsoc_spi_get_speed(spi* spi);
spi_bpw libsoc_spi_get_bits_per_word(spi* spi);

int libsoc_spi_write(spi* spi, uint8_t* tx, uint32_t len);
int libsoc_spi_read(spi* spi, uint8_t* rx, uint32_t len);
int libsoc_spi_rw(spi* spi, uint8_t* tx, uint8_t* rx, uint32_t len);

