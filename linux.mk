# install lightning on linux
PREFIX=/usr/local
BIN_DIR=$(PREFIX)/bin
LIB_DIR=$(PREFIX)/lib
SHARE_DIR=$(PREFIX)/share/lightning

LIGHTNINGD=lightningd
WWW=www

.PHONY: install

install:
	install -vC $(LIGHTNINGD) $(DESTDIR)/$(BIN_DIR)
	install -vC $(WWW) $(DESTDIR)/$(SHARE_DIR)
