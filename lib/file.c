#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

inline int file_open(const char *path, int flags)
{
  int fd = open(path, flags);

  if (fd < 0)
  {
    perror("libsoc-file-debug");
    return -1;
  }

  return fd;
}

inline int file_write(int fd, const char *str, int len)
{
  int ret_len = write(fd, str, len);

  if (ret_len < 0)
  {
    perror("libsoc-file-debug");
    return -1;
  }

  return ret_len;
}

inline int file_read(int fd, void *buf, int count)
{
  lseek (fd, 0, SEEK_SET);

  int ret = read(fd, buf, count);

  if (ret < 0)
  {
    perror("libsoc-file-debug");
    return -1;
  }

  return ret;
}

inline int file_valid(char *path)
{
  if (access(path, F_OK) == 0)
  {
    return 1;
  }

  return 0;
}

inline int file_close(int fd)
{
  if (close(fd) < 0)
  {
    perror ("libsoc-file-debug");
    return -1;
  }

  return 0;
}

#define INT_STR_BUF 20

inline int file_read_int_path(char *path, int *tmp)
{
  int fd, ret;

  fd = file_open(path, O_SYNC | O_RDONLY);

  if (fd < 0)
  {
    return EXIT_FAILURE;
  }

  ret = file_read_int_fd(fd, tmp);

  if (file_close(fd) < 0 || ret == EXIT_FAILURE)
  {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

inline int file_read_int_fd(int fd, int *tmp)
{
  char buf[INT_STR_BUF];

  if (file_read(fd, buf, INT_STR_BUF) < 0)
  {
    return EXIT_FAILURE;
  }

  *tmp = atoi(buf);

  return EXIT_SUCCESS;
}

inline int file_write_int_path(char *path, int val)
{
  int fd, ret;

  fd = file_open(path, O_SYNC | O_WRONLY);

  if (fd < 0)
  {
    return EXIT_FAILURE;
  }

  ret = file_write_int_fd(fd, val);

  if (file_close(fd) < 0 || ret == EXIT_FAILURE)
  {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

inline int file_write_int_fd(int fd, int val)
{
  char buf[INT_STR_BUF];

  sprintf(buf, "%d", val);

  if (file_write(fd, buf, INT_STR_BUF) < 0)
  {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

inline int file_read_str(char *path, char *tmp, int buf_len)
{
  int fd;

  fd = file_open(path, O_SYNC | O_RDONLY);

  if (fd < 0)
  {
    return EXIT_FAILURE;
  }

  if (file_read(fd, tmp, buf_len) < 0)
  {
    return EXIT_FAILURE;
  }

  if (file_close(fd) < 0)
  {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

inline int file_write_str(char *path, char* buf, int len)
{
  int fd;

  fd = file_open(path, O_SYNC | O_WRONLY);

  if (fd < 0)
  {
    return EXIT_FAILURE;
  }

  if (file_write(fd, buf, len) < 0)
  {
    return EXIT_FAILURE;
  }

  if (file_close(fd) < 0)
  {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

