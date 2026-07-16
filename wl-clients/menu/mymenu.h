#ifndef MENU_H
#define MENU_H

#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>

typedef struct {
  struct wl_display* display;
  struct wl_registry* registry;
  unsigned int lines;
  char* font;
  char* prompt;
  bool debug;
} wl_menu_state;

#endif  // !MENU_H
