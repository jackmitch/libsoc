libsocdir = $(pyexecdir)/libsoc
libsoc_PYTHON = __init__.py gpio.py

pyconfig = $(shell which $(target_cpu)-$(target_os)-python$(PYTHON_VERSION)-config || which python$(PYTHON_VERSION)-config)
AM_CPPFLAGS = $(shell $(pyconfig) --includes) -I${top_srcdir}/lib/include

libsoc_LTLIBRARIES = _libsoc.la
_libsoc_la_LDFLAGS = -module -avoid-version -export-dynamic
_libsoc_la_SOURCES = gpio_python.c
