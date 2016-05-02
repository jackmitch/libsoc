ACLOCAL_AMFLAGS = -I config/m4
AUTOMAKE_OPTIONS = subdir-objects

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libsoc.pc
EXTRA_DIST = libsoc.pc.in
CLEANFILES = libsoc.pc

SUBDIRS=lib contrib/board_files

if HAVE_PYTHON
SUBDIRS += bindings/python
endif
