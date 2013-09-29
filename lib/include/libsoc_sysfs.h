int sysfs_open(const char* path, int flags);
inline int sysfs_write(int fd, const char* str, int len);
inline int sysfs_read(int fd, void *buf, int count);
inline int sysfs_valid(char* path);
inline int sysfs_close(int fd);
