lib_LTLIBRARIES = libsoc.la
include_HEADERS = include/libsoc_gpio.h \
                  include/libsoc_spi.h \
                  include/libsoc_i2c.h \
                  include/libsoc_pwm.h \
                  include/libsoc_board.h \
                  include/libsoc_debug.h

libsoc_la_SOURCES = gpio.c \
										spi.c \
										file.c \
										i2c.c \
										pwm.c \
										board.c \
										debug.c

libsoc_la_CPPFLAGS = -I${top_srcdir}/lib/include

## interface : source : age

libsoc_la_LDFLAGS = -version-info 4:5:2
AM_CFLAGS = -DGPIO_CONF=\"@sysconfdir@/libsoc_gpio.conf\"
