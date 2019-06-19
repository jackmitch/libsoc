/* SPDX-License-Identifier: LGPL-2.1-only */

#ifndef _LIBSOC_FILE_H_
#define _LIBSOC_FILE_H_

#ifdef __cplusplus
extern "C" {
#endif

int file_open(const char* path, int flags);
int file_write(int fd, const char* str, int len);
int file_read(int fd, void *buf, int count);
int file_valid(char* path);
int file_close(int fd);
int file_write_int(char *path, int val);
int file_read_str(char *path, char *tmp, int buf_len);
int file_write_str(char *path, char* buf, int len);
int file_read_int(char *path, int *tmp);
int file_read_int_fd(int fd, int *tmp);
int file_write_int_fd(int fd, int val);
int file_write_int_path(char *path, int val);
char* file_read_contents(const char *path);

#ifdef __cplusplus
}
#endif
#endif
