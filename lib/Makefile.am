lib_LTLIBRARIES = libsoc.la
include_HEADERS = include/libsoc_gpio.h \
									include/libsoc_debug.h

libsoc_la_SOURCES = gpio.c \
										sysfs.c \
										debug.c

libsoc_la_CPPFLAGS = -I${top_srcdir}/lib/include

## interface : source : age

libsoc_la_LDFLAGS = -version-info 2:0:0


