#include "brightness.h"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

static char* root = "/sys/class";

static char* classes[] = {"backlight", "leds", NULL};

void
free_devices(struct device* devs) {
  struct device* dev = devs;
  while (dev->id != NULL) {
    free(dev->id);
    dev++;
  }
  free(devs);
}

int
get_all_device(struct device** devs_ptr, char* class_filter,
               char* device_filter) {
  unsigned int dev_size = 16;

  *devs_ptr = calloc(dev_size, sizeof(struct device));
  if (*devs_ptr == NULL) {
    LOG_ERR("Initial memory allocation failed.\n");
    return -1;
  }

  unsigned int dev_count = 0;
  char* cls_path = NULL;
  DIR* dptr = NULL;

  for (int i = 0; i < 2; i++) {
    char* cls = classes[i];

    if (class_filter != NULL && strcmp(cls, class_filter)) {
      continue;
    }

    unsigned int cls_path_len = strlen(root) + 1 + strlen(cls);
    cls_path = malloc(cls_path_len + 1);
    sprintf(cls_path, "%s/%s", root, cls);

    if ((dptr = opendir(cls_path)) == NULL) {
      LOG_ERR("Unable to read class: %s\n", cls);
      goto clean;
    }

    struct dirent* entry;

    while ((entry = readdir(dptr))) {
      char* id = entry->d_name;
      if (!strcmp(id, ".") || !strcmp(id, "..")) {
        continue;
      }

      if (device_filter != NULL && strcmp(id, device_filter)) {
        continue;
      }

      if ((dev_count + 1) >= dev_size) {
        unsigned int old_size = dev_size;
        dev_size *= 2;

        struct device* temp =
            realloc(*devs_ptr, dev_size * sizeof(struct device));
        if (temp == NULL) {
          LOG_ERR("Memory reallocation failed.\n");
          goto clean;
        }
        *devs_ptr = temp;

        memset((*devs_ptr) + old_size, 0,
               (dev_size - old_size) * sizeof(struct device));
      }

      struct device* dev = (*devs_ptr) + dev_count;

      dev->cls = cls;
      dev->id = strdup(id);

      int dev_path_len = strlen(cls_path) + 1 + strlen(id);

      char* file_path = malloc(dev_path_len + 1 + strlen("max_brightness") + 1);

      sprintf(file_path, "%s/%s/brightness", cls_path, id);
      FILE* fptr;

      if ((fptr = fopen(file_path, "r")) == NULL) {
        LOG_ERR("Unable to read %s\n", file_path);
        free(file_path);
        goto clean;
      }
      fscanf(fptr, "%u", &dev->curr_brightness);
      fclose(fptr);

      sprintf(file_path, "%s/%s/max_brightness", cls_path, id);
      if ((fptr = fopen(file_path, "r")) == NULL) {
        LOG_ERR("Unable to read max brightness of device %s\n", id);
        free(file_path);
        goto clean;
      }
      fscanf(fptr, "%u", &dev->max_brightness);
      fclose(fptr);
      dev_count++;
    }

    free(cls_path);
    closedir(dptr);
  }
  return dev_count;
clean:
  free_devices(*devs_ptr);
  free(cls_path);
  closedir(dptr);
  return -1;
}

void
print_all_device(struct device* devs) {
  const char* header_fmt = "%-25s %-15s %s\n";
  const char* value_fmt = "%-25s %-15s %3d%%\n";

  fprintf(stdout, header_fmt, "Device", "Class", "Brightness");
  struct device* dev = devs;

  while (dev->id != NULL) {
    LOG(value_fmt, dev->id, dev->cls, get_brightness(dev));
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

  r = sd_bus_call_method(bus, "org.freedesktop.login1",
                         "/org/freedesktop/login1/session/auto",
                         "org.freedesktop.login1.Session", "SetBrightness",
                         &error, NULL, "ssu", dev->cls, dev->id, value);

  if (r < 0) {
    fprintf(stderr, "Failed to issue method call: %s\n", error.message);
  }

  sd_bus_error_free(&error);
  sd_bus_unref(bus);

  return r < 0 ? -1 : 0;
#else
  FILE* fptr;

  unsigned int fpath_len = strlen(root) + 1 + strlen(dev->cls) + 1 +
                           strlen(dev->id) + 1 + strlen("brightness");

  char* fpath = malloc(fpath_len + 1);

  sprintf(fpath, "%s/%s/%s/brightness", root, dev->cls, dev->id);

  if ((fptr = fopen(fpath, "r+")) == NULL) {
    LOG("Unable to open %s.\n", fpath);
    free(fpath);
    return -1;
  }

  fprintf(fptr, "%u", value);

  fclose(fptr);
  free(fpath);
  return 0;
#endif
}

unsigned int
get_brightness(struct device* dev) {
  return (dev->curr_brightness * 100) / dev->max_brightness;
}
