if BOARD_CONFIGS

BOARD_FILES = $(filter-out example.conf, $(wildcard $(srcdir)/*.conf))

libsocdir = $(pkgdatadir)
libsoc_DATA = $(BOARD_FILES)

if BOARD
installdirs-local:
	install -d $(DESTDIR)$(sysconfdir)

install-data-hook: installdirs-local
	ln -srf $(DESTDIR)$(pkgdatadir)/@board@.conf $(DESTDIR)$(sysconfdir)/libsoc.conf
endif

uninstall-local:
	-rmdir $(DESTDIR)$(pkgdatadir)
	-rm $(DESTDIR)$(sysconfdir)/libsoc.conf
	-rmdir $(DESTDIR)$(sysconfdir)
else
if BOARD

install-data-hook:
	install -D $(srcdir)/@board@.conf $(DESTDIR)$(sysconfdir)/libsoc.conf

uninstall-local:
	-rm $(DESTDIR)$(sysconfdir)/libsoc.conf
	-rmdir $(DESTDIR)$(sysconfdir)

endif
endif
