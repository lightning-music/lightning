# install lightning on linux
PREFIX=/usr/local
BIN_DIR=$(PREFIX)/bin
LIB_DIR=$(PREFIX)/lib
SHARE_DIR=$(PREFIX)/share/lightning

LIBLIGHTNING_AR=liblightning.a
LIGHTNINGD=lightningd
WWW=www

.PHONY: install

install:
    install -vC $(LIBLIGHTNING_AR) $(DESTDIR)/$(LIB_DIR)
	install -vC $(LIGHTNINGD) $(DESTDIR)/$(BIN_DIR)
	install -vC $(WWW) $(DESTDIR)/$(SHARE_DIR)
