#include "mymenu.h"

#include <argp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include <wayland-util.h>

#include "xdg_stub.h"

static error_t
parse_opt(int key, char* arg, struct argp_state* state) {
  wl_menu_state* mstate = state->input;

  switch (key) {
    case 'p':
      mstate->prompt = arg;
      break;
    case 'f':
      mstate->font = arg;
      break;
    case 'l':
      mstate->lines = atoi(arg);
      break;
    case 'd':
      mstate->debug = true;
      break;
    case ARGP_KEY_INIT:
      mstate->prompt = "> ";
      mstate->font = "monospace 10";
      mstate->lines = 0;
      mstate->debug = false;
      break;
    case ARGP_KEY_END:
      break;
    default:
      return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp_option options[] = {  //
    {"font", 'f', "\"fontname size\"", 0, 0, 0},
    {"prompt", 'p', "\"string\"", 0, 0, 0},
    {"lines", 'l', "num", 0, 0, 0},
    {"debug", 'd', NULL, OPTION_ARG_OPTIONAL, 0, 0},
    {0}};

static struct argp argp = {options, parse_opt, NULL, NULL, NULL, NULL, NULL};

static void
registry_handle_global(void* data, struct wl_registry* registry, uint32_t name,
                       const char* interface, uint32_t version) {
  printf("interface: '%s', version: %d, name: %d\n", interface, version, name);
}

static void
registry_handle_global_remove(void* data, struct wl_registry* registry,
                              uint32_t name) {
  // This space deliberately left blank
}

static const struct wl_registry_listener registry_listener = {
    .global = registry_handle_global,
    .global_remove = registry_handle_global_remove,
};

int
main(int argc, char* argv[]) {
  wl_menu_state state = {0};

  if (argp_parse(&argp, argc, argv, 0, NULL, &state) != 0) {
    return EXIT_FAILURE;
  }

  if (state.debug) {
    fprintf(stdout,
            "Settings:\n\n"
            "Debug\t %b\n"
            "Font\t %s\n"
            "Prompt\t %s\n"
            "Lines\t %d\n"
            "\n",
            state.debug, state.font, state.prompt, state.lines);
  }

  if (!(state.display = wl_display_connect(NULL))) {
    fprintf(stderr, "Failed to connect to Wayland display\n");
    return EXIT_FAILURE;
  }

  if (!(state.registry = wl_display_get_registry(state.display))) {
    fprintf(stderr, "Failed to get registry for display\n");
    return EXIT_FAILURE;
  }

  wl_registry_add_listener(state.registry, &registry_listener, NULL);
  wl_display_roundtrip(state.display);

  while (wl_display_dispatch(state.display) != -1) {
  }

  fprintf(stderr, "Error dispatching Wayland events\n");
  wl_display_disconnect(state.display);
  return EXIT_SUCCESS;
}
