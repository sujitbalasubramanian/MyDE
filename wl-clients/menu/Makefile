include ../../global.mk

PKG_CFLAGS := $(shell pkg-config --cflags wayland-client)
PKG_LIBS := $(shell pkg-config --libs wayland-client)

CFLAGS += $(PKG_CFLAGS) -I.
LDLIBS := $(PKG_LIBS)

WAYLAND_SCANNER := $(shell pkg-config --variable=wayland_scanner wayland-scanner)
WLR_PROTOCOLS_DIR := $(shell pkg-config --variable=pkgdatadir wlr-protocols)

ifeq ($(strip $(WAYLAND_SCANNER)),)
    $(error "wayland-scanner executable not found via pkg-config")
endif

ifeq ($(strip $(WLR_PROTOCOLS_DIR)),)
    $(error "Could not find wlr-protocols pkgdatadir")
endif

LAYER_SHELL_XML := $(WLR_PROTOCOLS_DIR)/unstable/wlr-layer-shell-unstable-v1.xml
LAYER_SHELL_HEADER := wlr-layer-shell-unstable-v1-client-protocol.h
LAYER_SHELL_CODE := wlr-layer-shell-unstable-v1-protocol.c

.PHONY: all clean

all: mymenu

$(LAYER_SHELL_HEADER): $(LAYER_SHELL_XML)
	$(WAYLAND_SCANNER) client-header $< $@

$(LAYER_SHELL_CODE): $(LAYER_SHELL_XML)
	$(WAYLAND_SCANNER) private-code $< $@

mymenu: mymenu.c $(LAYER_SHELL_CODE) $(LAYER_SHELL_HEADER)
	$(CC) $(CFLAGS) -o $@ mymenu.c $(LAYER_SHELL_CODE) $(LDLIBS)

clean:
	rm -f *.o mymenu $(LAYER_SHELL_HEADER) $(LAYER_SHELL_CODE)
