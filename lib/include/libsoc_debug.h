#ifdef __cplusplus
extern "C" {
#endif

inline void libsoc_debug(const char *func, char *format, ...);
int libsoc_get_debug();
void libsoc_set_debug(int level);

#ifdef __cplusplus
}
#endif
