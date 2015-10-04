#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "libsoc_board.h"

#define _write(fd, buf) write(fd, buf, sizeof(buf)-1)

int main(void)
{
  int fails = 0;
  char template[] = "/tmp/fileXXXXXX";
  int fd = mkstemp(template);
  board_config *config;
  unsigned int id;

  _write(fd, "# comment line\n");
  _write(fd, "GPIO_FOO= 123\n");
  _write(fd, "\n");
  _write(fd, "GPIO_BAR =42 \n");
  _write(fd, "GPIO_B = 421 \n");
  _write(fd, "GPIO_C =21 \n");
  close(fd);

  setenv("LIBSOC_GPIO_CONF", template, 1);
  config = libsoc_board_init();


  id = libsoc_board_gpio_id(config, "GPIO_BAR");
  if (id != 42)
    {
      printf("ERROR: GPIO_BAR %d != 42\n", id);
      fails++;
    }
  id = libsoc_board_gpio_id(config, "GPIO_FOO");
  if (id != 123)
    {
      printf("ERROR: GPIO_FOO %d != 123\n", id);
      fails++;
    }
  id = libsoc_board_gpio_id(config, "GPIO_B");
  if (id != 421)
    {
      printf("ERROR: GPIO_B %d != 421\n", id);
      fails++;
    }
  id = libsoc_board_gpio_id(config, "GPIO_C");
  if (id != 21)
    {
      printf("ERROR: GPIO_C %d != 21\n", id);
      fails++;
    }

  printf("Tests completed with %d failure(s).\n", fails);
  libsoc_board_free(config);
  unlink(template);
  return fails;
}

