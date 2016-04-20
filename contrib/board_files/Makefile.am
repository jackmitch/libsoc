BOARD_FILES = $(shell cd $(srcdir); find ./ -name \*.conf | grep -v example)

if BOARD_CONFIGS
libsocdir = $(pkgdatadir)
libsoc_DATA = $(BOARD_FILES)

if BOARD
install-data-local:
# Delete current libsoc.conf if it exists, then symlink the selected board
	files=libsoc.conf ;\
	dir='$(DESTDIR)$(sysconfdir)'; $(am__uninstall_files_from_dir) ;\
	ln -s "$(DESTDIR)$(pkgdatadir)/@board@.conf" "$(sysconfdir)/libsoc.conf"
endif

uninstall-local:
	if test -z "$$(ls $(DESTDIR)$(pkgdatadir))" ; then \
		rmdir "$(DESTDIR)$(pkgdatadir)" ;\
	fi
if BOARD
	files=libsoc.conf ;\
	dir='$(DESTDIR)$(sysconfdir)'; $(am__uninstall_files_from_dir)
endif
else
if BOARD
# Just copy the one board config to etc.
install-data-local:
	files=libsoc.conf ;\
	cp "$(srcdir)/@board@.conf" "$(sysconfdir)/libsoc.conf"
uninstall-local:
	files=libsoc.conf ;\
	dir='$(DESTDIR)$(sysconfdir)'; $(am__uninstall_files_from_dir) ;
endif
endif

