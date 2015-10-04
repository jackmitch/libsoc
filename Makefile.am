ACLOCAL_AMFLAGS = -I config/m4
AUTOMAKE_OPTIONS = subdir-objects

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libsoc.pc
EXTRA_DIST = libsoc.pc.in
CLEANFILES = libsoc.pc

SUBDIRS=lib
if BOARD
SUBDIRS += contrib/board_files
endif
