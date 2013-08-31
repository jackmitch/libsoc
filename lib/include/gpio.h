typedef struct 
{
  unsigned int gpio;
  int value_fd;
} gpio;

typedef enum
{
  INPUT = 0,
  OUTPUT = 1
} gpio_direction;

typedef enum
{
  HIGH = 1,
  LOW = 0
} gpio_level;

gpio* libsoc_gpio_request(unsigned int gpio_id);
int libsoc_gpio_free(gpio* gpio);
int libsoc_gpio_set_direction(gpio* current_gpio, gpio_direction direction);
gpio_direction libsoc_gpio_get_direction(gpio* current_gpio);
int libsoc_gpio_set_level(gpio* current_gpio, gpio_level level);
gpio_level libsoc_gpio_get_level(gpio* current_gpio);
void libsoc_gpio_set_debug(int level);
