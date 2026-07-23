#include "brightness.h"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char* root = "/sys/class";

static char* classes[] = {"backlight", "leds", NULL};

void
free_devices(struct device** devs) {
  struct device* dev = *devs;
  while (dev->dev_id != NULL) {
    free(dev->dev_class);
  }
  free(devs);
}

int
get_all_device(struct device** devs, char* class_filter,
               char* deviceid_filter) {
  *devs = calloc(255, sizeof(struct device));
  int cls_idx = 0;
  char* cls;

  char* cls_path;
  DIR* dptr;
  FILE* fptr;

  int device_count = 0;

  while ((cls = classes[cls_idx++])) {
    if (class_filter != NULL && strcmp(cls, class_filter)) {
      continue;
    }

    cls_path = malloc(strlen(root) + 1 + strlen(cls) + 1);
    sprintf(cls_path, "%s/%s", root, cls);
    dptr = opendir(cls_path);

    if (dptr == NULL) {
      LOG_ERR("Unable to read class: %s\n", cls);
      goto clean;
    }

    struct dirent* entry;

    while ((entry = readdir(dptr))) {
      char* dev_id = entry->d_name;
      if (!strcmp(dev_id, ".") || !strcmp(dev_id, "..")) {
        continue;
      }

      if (deviceid_filter != NULL && strcmp(dev_id, deviceid_filter)) {
        continue;
      }

      // TODO: fix buffer overflow bug
      struct device* dev = &(*devs)[device_count++];
      dev->dev_class = cls;
      dev->dev_id = strdup(dev_id);

      int device_path_length = strlen(cls_path) + 1 + strlen(dev_id);

      char* file_path =
          malloc(device_path_length + 1 + strlen("brightness") + 1);

      sprintf(file_path, "%s/%s/brightness", cls_path, dev_id);

      if ((fptr = fopen(file_path, "r")) == NULL) {
        LOG_ERR("Unable to read curr brightness of device %s\n", dev_id);
        free(file_path);
        goto clean;
      }
      fscanf(fptr, "%u", &dev->curr_brightness);
      fclose(fptr);

      if ((file_path = realloc(file_path, device_path_length + 1 +
                                              strlen("max_brightness") + 1)) ==
          NULL) {
        LOG_ERR("memory allocation failed!\n");
        free(file_path);
        goto clean;
      }

      sprintf(file_path, "%s/%s/max_brightness", cls_path, dev_id);

      if ((fptr = fopen(file_path, "r")) == NULL) {
        LOG_ERR("Unable to read max brightness of device %s\n", dev_id);
        free(file_path);
        goto clean;
      }
      fscanf(fptr, "%u", &dev->max_brightness);
      fclose(fptr);
    }

    free(cls_path);
    closedir(dptr);
  }
  return device_count;
clean:
  free_devices(devs);
  free(cls_path);
  if (dptr) closedir(dptr);
  if (fptr) fclose(fptr);
  return -1;
}

void
print_all_device(struct device** devs) {
  const char* header_fmt = "%-25s %-15s %s\n";
  const char* value_fmt = "%-25s %-15s %3d%%\n";

  fprintf(stdout, header_fmt, "Device", "Class", "Brightness");
  struct device* dev = *devs;

  while (dev->dev_class != NULL) {
    fprintf(stdout, value_fmt, dev->dev_id, dev->dev_class,
            get_brightness(dev));
    dev++;
  }
}

int
parse_arg_percent_to_brightness_value(struct device* dev, char* arg_str) {
  if (arg_str == NULL) {
    return -1;
  }

  char* endptr;
  errno = 0;

  long val = strtol(arg_str, &endptr, 10);

  if (arg_str == endptr || *endptr != '\0' || errno == ERANGE) {
    return -1;
  }

  if (*arg_str == '+' || *arg_str == '-') {
    val = get_brightness(dev) + val;
  }

  val = val < 0 ? 0 : val;
  val = val > 100 ? 100 : val;

  return (dev->max_brightness * val) / 100;
}

int
set_brightness(struct device* dev, unsigned int value) {
#ifdef USE_LOGIND
  sd_bus_error error = SD_BUS_ERROR_NULL;
  sd_bus* bus = NULL;
  int r;

  r = sd_bus_open_system(&bus);
  if (r < 0) {
    fprintf(stderr, "Failed to connect to system bus: %s\n", strerror(-r));
    return r;
  }

  r = sd_bus_call_method(
      bus, "org.freedesktop.login1", "/org/freedesktop/login1/session/auto",
      "org.freedesktop.login1.Session", "SetBrightness", &error, NULL, "ssu",
      dev->dev_class, dev->dev_id, value);

  if (r < 0) {
    fprintf(stderr, "Failed to issue method call: %s\n", error.message);
  }

  sd_bus_error_free(&error);
  sd_bus_unref(bus);

  return r < 0 ? -1 : 0;
#else
  // TODO: implement write brightness
#endif
}

unsigned int
get_brightness(struct device* dev) {
  return (dev->curr_brightness * 100) / dev->max_brightness;
}
