# liblightning shared library
# Brian Sorahan 2014
include flags.mk
include objs.mk
include vars.mk

.PHONY: all install

all .DEFAULT: $(LIBLIGHTNING_SO)

SHARED_OBJS := $(addprefix $(SHARED_DIR)/,$(OBJS))

$(LIBLIGHTNING_SO): $(SHARED_DIR) $(SHARED_OBJS)
	gcc -shared -o $(LIBLIGHTNING_SO) $(SHARED_OBJS)

$(SHARED_DIR):
	mkdir $(SHARED_DIR)

install: $(LIBLIGHTNING_SO) $(includedir)
	install -vC $(LIBLIGHTNING_SO) $(DESTDIR)$(libdir)

$(includedir):
	mkdir $(includedir)

.shared/%.o: %.c
	gcc -fPIC -Wall -g -O2 -c -o $@ $^
