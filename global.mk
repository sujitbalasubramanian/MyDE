CC = cc
CFLAGS := -std=c23 -Wall -Wextra

BUILD ?= debug

ifeq ($(BUILD),debug)
    CFLAGS += -g -O0 -DDEBUG
else ifeq ($(BUILD),release)
    CFLAGS += -O3 -DNDEBUG
else
    $(error Invalid BUILD type: '$(BUILD)'. Use 'debug' or 'release')
endif
