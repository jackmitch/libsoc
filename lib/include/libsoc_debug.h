#ifndef _LIBSOC_DEBUG_H_
#define _LIBSOC_DEBUG_H_

#ifdef __cplusplus
extern "C" {
#endif

void libsoc_debug(const char *func, char *format, ...) __attribute__((format(printf, 2, 3)));
void libsoc_warn(const char *format, ...) __attribute__((format(printf, 1, 2)));
void libsoc_error(const char *func, const char *format, ...) __attribute__((format(printf, 2, 3)));
int libsoc_get_debug();
void libsoc_set_debug(int level);

#ifdef __cplusplus
}
#endif
#endif
