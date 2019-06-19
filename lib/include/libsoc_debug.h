/* SPDX-License-Identifier: LGPL-2.1-only */

#ifndef _LIBSOC_DEBUG_H_
#define _LIBSOC_DEBUG_H_

#ifdef __cplusplus
extern "C" {
#endif

void libsoc_debug(const char *func, char *format, ...) __attribute__((format(printf, 2, 3)));
void libsoc_warn(const char *format, ...) __attribute__((format(printf, 1, 2)));
int libsoc_get_debug();
void libsoc_set_debug(int level);

#ifdef __cplusplus
}
#endif
#endif
