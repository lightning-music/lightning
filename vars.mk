prefix=/usr/local
bindir=$(prefix)/bin
includedir=$(prefix)/include/lightning
libdir=$(prefix)/lib
# headers
HEADERS := lightning.h types.h
# examples, archive
LIBLIGHTNING_AR=liblightning.a
LIBLIGHTNING_SO=liblightning-0.1.0.so
# directory to hold object files for shared library
SHARED_DIR=.shared
EXAMPLES := play-file play-sample lightning-play-sample
EXAMPLES := $(addprefix examples/,$(EXAMPLES))
# tests
TEST_DIR=test
TESTS := check-list check-metro
TESTS := $(addprefix $(TEST_DIR)/, $(TESTS))
# API docs
API_DOCS_DIR=/var/www/html/liblightning
