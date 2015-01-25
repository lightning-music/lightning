LIBLIGHTNING_GIT=https://github.com/lightning/liblightning.git
LIBLIGHTNING_DIR=liblightning
LIGHTNINGGO_GIT=https://github.com/lightning/go.git
GOHOME=$(GOPATH)/src/github.com/lightning
LIGHTNINGGO_DIR=$(GOHOME)/go
LIGHTNINGD_GIT=https://github.com/lightning/lightningd.git
LIGHTNINGD_DIR=$(GOHOME)/lightningd
WWW_GIT=https://github.com/lightning/www.git

PKG_DIR=lightning
PKG=$(PKG_DIR).tar.gz
README=README.md
DATE=$(shell date +'%Y.%m.%d')

.PHONY: linux_amd64 clean install

install:
	cd $(LIBLIGHTNING_DIR) && make && make install
	cd $(LIGHTNINGGO_DIR) && go install -a
	cd $(LIGHTNINGD_DIR) && go install -a

# Build binary package for 64-bit linux
linux_amd64: $(GOHOME) $(PKG_DIR) $(LIGHTNINGD_DIR) $(LIGHTNINGGO_DIR) $(LIBLIGHTNING_DIR)
	cd $(LIBLIGHTNING_DIR) && make
	cd $(LIGHTNINGGO_DIR) && go install -a
	cd $(LIGHTNINGD_DIR) && make
	cp $(LIGHTNINGD_DIR)/lightningd $(PKG_DIR)
	cd $(PKG_DIR) && git clone $(WWW_GIT)
	cp LICENSE README.md $(PKG_DIR)
	cp linux_amd64.mk $(PKG_DIR)/Makefile
	tar czf $(PKG_DIR)-$(DATE).tar.gz $(PKG_DIR)

$(PKG_DIR):
	mkdir $(PKG_DIR)

$(GOHOME):
	mkdir -p $(GOHOME)

$(LIBLIGHTNING_DIR):
	git clone $(LIBLIGHTNING_GIT)

$(LIGHTNINGD_DIR):
	cd $(GOHOME) && git clone $(LIGHTNINGD_GIT)

$(LIGHTNINGGO_DIR):
	cd $(GOHOME) git clone $(LIGHTNINGGO_GIT)

clean:
	rm -rf *~ $(PKG_DIR) $(LIBLIGHTNING_DIR)
