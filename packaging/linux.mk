# install lightning on linux
PREFIX=$(DESTDIR)/usr/local
BIN_DIR=$(PREFIX)/bin
LIB_DIR=$(PREFIX)/lib
SHARE_DIR=$(PREFIX)/share/lightning

LIGHTNINGD=lightningd
WWW=www

.PHONY: install

install:
	install -vC $(LIGHTNINGD) $(BIN_DIR)
	mkdir -p $(SHARE_DIR)
	rm -rf $(SHARE_DIR)/$(WWW)
	cp -R $(WWW) $(SHARE_DIR)
