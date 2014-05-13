#ifndef _LIBSOC_FILE_H_
#define _LIBSOC_FILE_H_

#ifdef __cplusplus
extern "C" {
#endif

int file_open(const char* path, int flags);
inline int file_write(int fd, const char* str, int len);
inline int file_read(int fd, void *buf, int count);
inline int file_valid(char* path);
inline int file_close(int fd);
inline int file_write_int(char *path, int val);
inline int file_read_str(char *path, char *tmp, int buf_len);
inline int file_write_str(char *path, char* buf, int len);
inline int file_read_int(char *path, int *tmp);

#ifdef __cplusplus
}
#endif
#endif
