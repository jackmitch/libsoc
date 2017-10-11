libsocdir = $(pyexecdir)/libsoc
libsoc_PYTHON = __init__.py gpio.py i2c.py spi.py

AM_CPPFLAGS = $(PYTHON_CFLAGS) -I${top_srcdir}/lib/include -DLIBSOC_SO=\"libsoc.so.2\"

libsoc_LTLIBRARIES = _libsoc.la
_libsoc_la_LDFLAGS = -module -avoid-version -export-dynamic $(PYTHON_LIBS)
_libsoc_la_SOURCES = libsoc_python.c
