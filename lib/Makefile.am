lib_LTLIBRARIES = libsoc.la
include_HEADERS = include/libsoc_gpio.h \
									include/libsoc_spi.h \
									include/libsoc_i2c.h \
									include/libsoc_debug.h

libsoc_la_SOURCES = gpio.c \
										spi.c \
										file.c \
										i2c.c \
										debug.c

libsoc_la_CPPFLAGS = -I${top_srcdir}/lib/include

## interface : source : age

libsoc_la_LDFLAGS = -version-info 3:0:0


