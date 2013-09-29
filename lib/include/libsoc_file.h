int file_open(const char* path, int flags);
inline int file_write(int fd, const char* str, int len);
inline int file_read(int fd, void *buf, int count);
inline int file_valid(char* path);
inline int file_close(int fd);
