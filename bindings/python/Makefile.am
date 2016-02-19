libsocdir = $(pyexecdir)/libsoc
libsoc_PYTHON = __init__.py gpio.py

AM_CPPFLAGS = $(PYTHON_CFLAGS) -I${top_srcdir}/lib/include

libsoc_LTLIBRARIES = _libsoc.la
_libsoc_la_LDFLAGS = -module -avoid-version -export-dynamic
_libsoc_la_SOURCES = gpio_python.c
