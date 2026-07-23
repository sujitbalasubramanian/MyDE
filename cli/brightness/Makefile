include ../../global.mk

CFLAGS += -I.
LD_FLAGS :=

BIN_DIR := /usr/local/bin
MODE := 4711

USE_LOGIND ?= true

ifeq ($(USE_LOGIND),true)
	CFLAGS += -DUSE_LOGIND $(pkg-config --cflags libsystemd)
	LD_FLAGS += $(shell pkg-config --libs libsystemd)
	MODE = 0755
endif

.PHONY: all clean

all: brightness

install: all
	install -m ${MODE} brightness ${BIN_DIR}/

brightness: main.c brightness.c
	$(CC) $(CFLAGS) -o $@ $^ $(LD_FLAGS)

clean:
	rm -rf *.o brightness
