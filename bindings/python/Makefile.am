libsocdir = $(pyexecdir)/libsoc
libsoc_PYTHON = __init__.py gpio.py

AM_CPPFLAGS = -I/usr/include/python$(PYTHON_VERSION) -I../../include/lib

libsoc_LTLIBRARIES = _libsoc.la
_libsoc_la_LDFLAGS = -lsoc -module -avoid-version -export-symbols-regex init_libsoc
_libsoc_la_SOURCES = gpio_python.c
