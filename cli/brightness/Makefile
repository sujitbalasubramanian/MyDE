include ../../global.mk

CFLAGS += -I. -I../../libs/
LD_FLAGS :=

BIN_DIR := /usr/local/bin
# use 4711 for root mode
MODE ?= 0755

USE_LOGIND ?= true

ifeq ($(USE_LOGIND),true)
	CFLAGS += -DUSE_LOGIND $(pkg-config --cflags libsystemd)
	LD_FLAGS += $(shell pkg-config --libs libsystemd)
endif

.PHONY: all clean

all: brightness

install: all
	install -m ${MODE} brightness ${BIN_DIR}/

install_udev:
	install -m 0644 90-brightness.rules /usr/lib/udev/rules.d

brightness: main.c brightness.c
	$(CC) $(CFLAGS) -o $@ $^ $(LD_FLAGS)

clean:
	rm -rf *.o brightness
