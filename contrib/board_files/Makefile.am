BOARD_FILES = $(shell cd $(srcdir); find ./ -name \*.conf)

if BOARD_CONFIGS
# Copy all board files to share/libsoc/...
install-data-local:
	set -x ;\
	pushd $(srcdir) ;\
	for p in $(BOARD_FILES); do \
		$(INSTALL_DATA) -m 755 -d "$(DESTDIR)$(pkgdatadir)/$$(dirname $$p)";\
		$(INSTALL_DATA) $$p "$(DESTDIR)$(pkgdatadir)/$$(dirname $$p)/" || exit $$?;\
	done ; popd ; set +x
if BOARD
# Delete current libsoc_gpio.conf if it exists, then symlink the selected board
	files=libsoc_gpio.conf ;\
	dir='$(DESTDIR)$(sysconfdir)'; $(am__uninstall_files_from_dir) ;\
	ln -s "$(DESTDIR)$(pkgdatadir)/@board@/libsoc_gpio.conf" "$(sysconfdir)/"
endif

uninstall-local:
	files="$(BOARD_FILES)" ;\
	dir='$(DESTDIR)$(pkgdatadir)'; $(am__uninstall_files_from_dir)
	for p in $(BOARD_FILES); do \
		if test -z "$$(ls $(DESTDIR)$(pkgdatadir)/$$(dirname $$p))" ; then \
			rmdir "$(DESTDIR)$(pkgdatadir)/$$(dirname $$p)" ;\
		fi ;\
	done ;\
	if test -z "$$(ls $(DESTDIR)$(pkgdatadir))" ; then \
		rmdir "$(DESTDIR)$(pkgdatadir)" ;\
	fi
if BOARD
	files=libsoc_gpio.conf ;\
	dir='$(DESTDIR)$(sysconfdir)'; $(am__uninstall_files_from_dir)
endif
else
if BOARD
# Just copy the one board config to etc.
sysconf_DATA = @board@/libsoc_gpio.conf
endif
endif
