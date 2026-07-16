include ../global.mk

PKG_CFLAGS := $(shell pkg-config --cflags wayland-server)
PKG_LIBS := $(shell pkg-config --libs wayland-server)

CFLAGS += $(PKG_CFLAGS)
LDLIBS := $(PKG_LIBS)

.PHONY: all clean

all: mycomp

mycomp: mycomp.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -rf *.o mycomp
