#include "mycomp.h"

#include <argp.h>
#include <stdio.h>
#include <stdlib.h>
#include <wayland-server.h>
#include <wayland-util.h>

static error_t
parse_opt(int key, char* arg, struct argp_state* state) {
  wl_comp_state* wl_comp_state = state->input;
  switch (key) {
    case 'c':
      wl_comp_state->config = arg;
      break;
    case 'd':
      wl_comp_state->debug = true;
      break;
    case ARGP_KEY_INIT:
      // TODO: add xdg config path
      wl_comp_state->config = NULL;
      wl_comp_state->debug = false;
      break;
    default:
      return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp_option options[] = {  //
    {"config", 'c', "<config file>", 0, 0, 0},
    {"debug", 'd', 0, OPTION_ARG_OPTIONAL, 0, 0},
    {0}};

static struct argp argp = {options, parse_opt, NULL, NULL, NULL, NULL, NULL};

int
main(int argc, char* argv[]) {
  wl_comp_state state = {0};
  if (argp_parse(&argp, argc, argv, 0, NULL, &state) != 0) {
    return EXIT_FAILURE;
  }

  if (state.debug) {
    fprintf(stdout,
            "Settings:\n\n"
            "Config File\t%s\n"
            "\n",
            state.config);
  }

  struct wl_display* display = wl_display_create();
  if (!display) {
    fprintf(stderr, "Unable to create Wayland display.\n");
    return 1;
  }

  const char* socket = wl_display_add_socket_auto(display);
  if (!socket) {
    fprintf(stderr, "Unable to add socket to Wayland display.\n");
    return 1;
  }

  fprintf(stderr, "Running Wayland display on %s\n", socket);
  wl_display_run(display);

  wl_display_destroy(display);
  return EXIT_SUCCESS;
}
