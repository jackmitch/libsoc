#include <unistd.h>

inline int sysfs_open(const char* path, int flags) 
{
	int fd = open(path, flags);

  if (fd < 0) {
    perror("libsoc-sysfs-debug");
		return -1;
	}
    
  return fd;
}

inline int sysfs_write(int fd, const char* str, int len) 
{
  int ret_len = write(fd, str, len);
  
  if (ret_len < 0) {
    perror("libsoc-sysfs-debug");
		return -1;
  }
  
  return ret_len;
}

inline int sysfs_read(int fd, void *buf, int count) 
{
  int ret = read(fd, buf, count);
  
  if (ret < 0) {
		perror("libsoc-sysfs-debug");
		return -1;
  }
  
  return ret;
}

inline int sysfs_valid(char* path)
{
  if (access(path, F_OK) == 0) {
    return 1;
  }
  
  return 0;
}

inline int sysfs_close(int fd)
{
  if (close(fd) < 0) {
    perror("libsoc-sysfs-debug");
    return -1;
  }
  
  return 0;
}
