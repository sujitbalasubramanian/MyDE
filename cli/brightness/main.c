#include <argp.h>
#include <stdlib.h>
#include <string.h>

#include "brightness.h"

const char* argp_program_bug_address = "sujitbalasubramanianr@gmail.com";
const char* argp_program_version = "version 0.1";

enum OPERATION { LIST, GET, SET };

struct app_ctx {
  enum OPERATION operation;
  char* value;
  char* dev_id;
  char* dev_class;
};

static error_t
parse_opt(int key, char* arg, struct argp_state* state) {
  struct app_ctx* app_ctx = state->input;

  switch (key) {
    case 'l':
      app_ctx->operation = LIST;
      break;
    case 'g':
      app_ctx->operation = GET;
      break;
    case 's':
      app_ctx->operation = SET;
      app_ctx->value = arg;
      break;
      break;
    case 'd':
      app_ctx->dev_id = arg;
      break;
    case 'c':
      if (strcmp(arg, "backlight") && strcmp(arg, "leds")) {
        argp_error(state, "invalid class backlight and leds are only allowed");
      }
      app_ctx->dev_class = arg;
      break;
    case ARGP_KEY_INIT:
      app_ctx->operation = LIST;
      app_ctx->value = NULL;
      app_ctx->dev_id = NULL;
      app_ctx->dev_class = NULL;
      break;
    default:
      return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp_option options[] = {
    {"list", 'l', 0, 0, "list available devices with brightness control.", 0},
    {"get", 'g', 0, 0, "get current brightness of the device.", 0},
    {"set", 's', "value", 0, "set brightness of the device. eg: 5, -5, +5", 0},
    {"device", 'd', "DEVICE", 0, "specify device name.", 0},
    {"class", 'c', "CLASS", 0, "specify class name.", 0},
    {0}};

static struct argp argp = {options, parse_opt, 0, 0, 0, 0, 0};

int
main(int argc, char* argv[]) {
  struct app_ctx ctx = {0};
  if (argp_parse(&argp, argc, argv, 0, 0, &ctx) != 0) {
    return EXIT_FAILURE;
  }

  struct device* devs = NULL;
  int device_count = 0;

  if ((device_count = get_all_device(&devs, ctx.dev_class, ctx.dev_id)) < 0) {
    fprintf(stdout, "Failed to fetch devices!\n");
    return EXIT_FAILURE;
  }

  if (device_count == 0) {
    fprintf(stdout, "No Devices Found!\n");
    return EXIT_FAILURE;
  }

  struct device dev = devs[0];
  int brightness;

  switch (ctx.operation) {
    case LIST:
      print_all_device(&devs);
      break;
    case GET:
      fprintf(stdout, "%d\n", get_brightness(&dev));
      break;
    case SET:
      if ((brightness =
               parse_arg_percent_to_brightness_value(&dev, ctx.value)) < 0) {
        fprintf(stdout, "Invalid input for -s|--set\n");
        return EXIT_FAILURE;
      }
      if ((brightness = set_brightness(&dev, brightness)) != 0) {
        return EXIT_FAILURE;
      }
      break;
  }

  return EXIT_SUCCESS;
}
