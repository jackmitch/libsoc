#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <poll.h>

#include "libsoc_gpio.h"

#define STR_BUF 256
#define MAX_GPIO_ID 255

const char gpio_level_strings[2][STR_BUF] = {"0", "1"};
const char gpio_direction_strings[2][STR_BUF] = {"in", "out"};
const char gpio_edge_strings[3][STR_BUF] = {"rising", "falling", "none"};

int debug = 0;

void libsoc_gpio_set_debug(int level)
{
  #ifdef DEBUG
  
  if (level)
  {
    debug = 1;
  }
  else
  {
    debug = 0;
  }
  
  #else
  
  printf("libsoc-gpio: warning debug support missing!\n");
    
  #endif
}

inline void libsoc_gpio_debug(const char* func, unsigned int gpio, char* format, ...)
{
  #ifdef DEBUG
  
  if (debug)
  {
    va_list args;
    
    fprintf(stderr, "libsoc-gpio-debug: ");

    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    
    if (gpio >= 0)
    {
      fprintf(stderr, " (%d, %s)", gpio, func);
    }
    else
    {
      fprintf(stderr, " (NULL, %s)", func);
    }
    
    fprintf(stderr, "\n");
  }
  
  #endif
}

gpio* libsoc_gpio_request(unsigned int gpio_id)
{
  gpio* new_gpio;
  int fd, ret;
  char tmp_str[STR_BUF];
  
  libsoc_gpio_debug(__func__, gpio_id, "requested gpio");
  
  if (gpio_id > MAX_GPIO_ID || gpio_id < 0)
  {
    libsoc_gpio_debug(__func__, gpio_id, "gpio out of range (0-255)");
    return NULL;
  }
  
  fd = open("/sys/class/gpio/export", O_SYNC | O_WRONLY);
  
  if (fd < 0)
  {
    libsoc_gpio_debug(__func__, gpio_id, "opening sysfs export failed");
    perror("libsoc-gpio-debug");
    return NULL;
  }
  
  sprintf(tmp_str, "%d", gpio_id);
  
  ret = write(fd, tmp_str, STR_BUF);
  
  if (ret == 0 || ret < 0)
  {
    libsoc_gpio_debug(__func__, gpio_id, "write to export failed");
    perror("libsoc-gpio-debug");
    return NULL;
  }
  
  close(fd);
  
  sprintf(tmp_str, "/sys/class/gpio/gpio%d", gpio_id);
  
  if (access(tmp_str, F_OK ) == -1)
  {
    libsoc_gpio_debug(__func__, gpio_id, "ACCESS FAIL\n");
    perror("libsoc-gpio-debug");
    return NULL;
  }
  
  new_gpio = malloc(sizeof(gpio));
  
  sprintf(tmp_str, "/sys/class/gpio/gpio%d/value", gpio_id);
  
  new_gpio->value_fd = open(tmp_str, O_SYNC | O_RDWR);
  
  if (new_gpio->value_fd < 0)
  {
    libsoc_gpio_debug(__func__, gpio_id, "opening gpio value failed");
    perror("libsoc-gpio-debug");
  }
  
  new_gpio->gpio = gpio_id;
  
  return new_gpio;
}

int libsoc_gpio_free(gpio* gpio)
{
  char tmp_str[STR_BUF];
  int fd;
  
  if (gpio == NULL)
  {
    libsoc_gpio_debug(__func__, -1, "invalid gpio pointer");
    return EXIT_FAILURE;
  }
  
  libsoc_gpio_debug(__func__, gpio->gpio, "freeing gpio");
  
  close(gpio->value_fd);
  
  fd = open("/sys/class/gpio/unexport", O_SYNC | O_WRONLY);
  
  if (fd < 0)
  {
    libsoc_gpio_debug(__func__, gpio->gpio, "opening sysfs unexport failed");
    perror("libsoc-gpio-debug");
    return EXIT_FAILURE;
  }
  
  sprintf(tmp_str, "%d", gpio->gpio);
  
  write(fd, tmp_str, STR_BUF);
  
  close(fd);
  
  sprintf(tmp_str, "/sys/class/gpio/gpio%d", gpio->gpio);
  
  if (access(tmp_str, F_OK ) != -1)
  {
    libsoc_gpio_debug(__func__, gpio->gpio, "freeing failed");
    return EXIT_FAILURE;
  }
  
  free(gpio);
  
  return EXIT_SUCCESS;
}

int libsoc_gpio_set_direction(gpio* current_gpio, gpio_direction direction)
{
  int fd;
  char path[STR_BUF];
  
  if (current_gpio == NULL)
  {
    libsoc_gpio_debug(__func__, -1, "invalid gpio pointer");
    return EXIT_FAILURE;
  }
  
  libsoc_gpio_debug(__func__, current_gpio->gpio, "setting direction");
  
  sprintf(path, "/sys/class/gpio/gpio%d/direction", current_gpio->gpio);
  
  fd = open(path, O_SYNC | O_WRONLY);
  
  if (fd < 0)
  {
    libsoc_gpio_debug(__func__, current_gpio->gpio, "opening sysfs direction failed");
    perror("libsoc-gpio-debug");
    return EXIT_FAILURE;
  }
  
  write(fd, gpio_direction_strings[direction], STR_BUF);
  
  close(fd);
  
  return EXIT_SUCCESS;
}

gpio_direction libsoc_gpio_get_direction(gpio* current_gpio)
{
  int fd;
  char tmp_str[STR_BUF];
  
  if (current_gpio == NULL)
  {
    libsoc_gpio_debug(__func__, -1, "invalid gpio pointer");
    return DIRECTION_ERROR;
  }
  
  libsoc_gpio_debug(__func__, current_gpio->gpio, "reading direction");
  
  sprintf(tmp_str, "/sys/class/gpio/gpio%d/direction", current_gpio->gpio);
  
  fd = open(tmp_str, O_RDONLY);
  
  if (fd < 0)
  {
    libsoc_gpio_debug(__func__, current_gpio->gpio, "opening sysfs direction failed");
    perror("libsoc-gpio-debug");
    return DIRECTION_ERROR;
  }
  
  lseek(fd, 0, SEEK_SET);
  
  read(fd, tmp_str, STR_BUF);
  
  close(fd);
  
  if (strncmp(tmp_str, "in", 2) <= 0)
  {
    libsoc_gpio_debug(__func__, current_gpio->gpio, "got direction as input");
    return INPUT;
  }
  else
  {
    libsoc_gpio_debug(__func__, current_gpio->gpio, "got direction as output");
    return OUTPUT;
  }
}

int libsoc_gpio_set_level(gpio* current_gpio, gpio_level level)
{
  if (current_gpio == NULL)
  {
    libsoc_gpio_debug(__func__, -1, "invalid gpio pointer");
    return EXIT_FAILURE;
  }
  
  libsoc_gpio_debug(__func__, current_gpio->gpio, "set level as %d", level);
  
  write(current_gpio->value_fd, gpio_level_strings[level], 1);
  
  return EXIT_SUCCESS;
}

gpio_level libsoc_gpio_get_level(gpio* current_gpio)
{
  char level[STR_BUF];
  int ret;
  
  if (current_gpio == NULL)
  {
    libsoc_gpio_debug(__func__, -1, "invalid gpio pointer");
    return LEVEL_ERROR;
  }
  
  libsoc_gpio_debug(__func__, current_gpio->gpio, "reading level");
  
  lseek(current_gpio->value_fd, 0, SEEK_SET);
  
  ret = read(current_gpio->value_fd, level, STR_BUF);
  
  if (ret < 0)
  {
    libsoc_gpio_debug(__func__, current_gpio->gpio, "level read failed");
    perror("libgpio");
    return LEVEL_ERROR;
  }
  
  if (strncmp(level, "0", 1) <= 0)
  {
    libsoc_gpio_debug(__func__, current_gpio->gpio, "got level as low");
    return LOW;
  }
  else
  {
    libsoc_gpio_debug(__func__, current_gpio->gpio, "got level as high");
    return HIGH;
  }
}

int libsoc_gpio_set_edge(gpio* current_gpio, gpio_edge edge)
{
  int fd;
  char path[STR_BUF];
  
  if (current_gpio == NULL)
  {
    libsoc_gpio_debug(__func__, -1, "invalid gpio pointer");
    return EXIT_FAILURE;
  }
  
  libsoc_gpio_debug(__func__, current_gpio->gpio, "setting edge to %s", gpio_edge_strings[edge]);
  
  sprintf(path, "/sys/class/gpio/gpio%d/edge", current_gpio->gpio);
  
  fd = open(path, O_SYNC | O_WRONLY);
  
  if (fd < 0)
  {
    libsoc_gpio_debug(__func__, current_gpio->gpio, "opening sysfs edge failed");
    perror("libsoc-gpio-debug");
    return EXIT_FAILURE;
  }
  
  write(fd, gpio_edge_strings[edge], STR_BUF);
  
  close(fd);
  
  return EXIT_SUCCESS;
}

gpio_edge libsoc_gpio_get_edge(gpio* current_gpio)
{
  int fd;
  char tmp_str[STR_BUF];
  
  if (current_gpio == NULL)
  {
    libsoc_gpio_debug(__func__, -1, "invalid gpio pointer");
    return EDGE_ERROR;
  }
  
  libsoc_gpio_debug(__func__, current_gpio->gpio, "reading edge");
  
  sprintf(tmp_str, "/sys/class/gpio/gpio%d/edge", current_gpio->gpio);
  
  fd = open(tmp_str, O_RDONLY);
  
  if (fd < 0)
  {
    libsoc_gpio_debug(__func__, current_gpio->gpio, "opening sysfs edge failed");
    perror("libsoc-gpio-debug");
    return EDGE_ERROR;
  }
  
  lseek(fd, 0, SEEK_SET);
  
  read(fd, tmp_str, STR_BUF);
  
  close(fd);
  
  if (strncmp(tmp_str, "r", 1) == 0)
  {
    libsoc_gpio_debug(__func__, current_gpio->gpio, "got edge as rising");
    return RISING;
  }
  else if (strncmp(tmp_str, "f", 1) == 0)
  {
    libsoc_gpio_debug(__func__, current_gpio->gpio, "got edge as falling");
    return FALLING;
  }
  else
  {
    libsoc_gpio_debug(__func__, current_gpio->gpio, "got edge as none");
    return NONE;
  }
}

int libsoc_gpio_wait_interrupt(gpio* gpio, int timeout)
{
  if (gpio == NULL)
  {
    libsoc_gpio_debug(__func__, -1, "invalid gpio pointer");
    return EDGE_ERROR;
  }
  
  if (libsoc_gpio_get_direction(gpio) != INPUT)
  {
    libsoc_gpio_debug(__func__, gpio->gpio, "gpio is not set as input");
    return EXIT_FAILURE;
  }
  
  gpio_edge test_edge = libsoc_gpio_get_edge(gpio);
  
  if ( test_edge == EDGE_ERROR || test_edge == NONE)
  {
    libsoc_gpio_debug(__func__, gpio->gpio, "edge must be FALLING or RISING");
    return EXIT_FAILURE;
  }
  
  struct pollfd pfd[1];

  pfd[0].fd = gpio->value_fd;
  pfd[0].events = POLLPRI;
  pfd[0].revents = 0;

  int ready = poll(pfd, 1, timeout);
  
  int ret;
  
  switch (ready)
  {
    case -1:
      libsoc_gpio_debug(__func__, gpio->gpio, "poll failed");
      perror("libsoc-gpio-debug");
      ret = EXIT_FAILURE;
      break;
      
    case 0:
      ret = EXIT_FAILURE;
      break;
    
    default:
      ret = EXIT_SUCCESS;
      break;
  }
  
  return ret;
  
}

