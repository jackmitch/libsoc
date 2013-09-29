#include <unistd.h>

inline int file_open(const char* path, int flags) 
{
	int fd = open(path, flags);

  if (fd < 0) {
    perror("libsoc-file-debug");
		return -1;
	}
    
  return fd;
}

inline int file_write(int fd, const char* str, int len) 
{
  int ret_len = write(fd, str, len);
  
  if (ret_len < 0) {
    perror("libsoc-file-debug");
		return -1;
  }
  
  return ret_len;
}

inline int file_read(int fd, void *buf, int count) 
{
  int ret = read(fd, buf, count);
  
  if (ret < 0) {
		perror("libsoc-file-debug");
		return -1;
  }
  
  return ret;
}

inline int file_valid(char* path)
{
  if (access(path, F_OK) == 0) {
    return 1;
  }
  
  return 0;
}

inline int file_close(int fd)
{
  if (close(fd) < 0) {
    perror("libsoc-file-debug");
    return -1;
  }
  
  return 0;
}
