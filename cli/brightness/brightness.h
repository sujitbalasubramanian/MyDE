#ifndef BRIGHTNESS_H
#define BRIGHTNESS_H

#ifdef USE_LOGIND
#include <systemd/sd-bus.h>
#endif

#define LOG_ERR(...) fprintf(stderr, "[error] " __VA_ARGS__);

struct device {
  char* dev_class;
  char* dev_id;
  unsigned int curr_brightness;
  unsigned int max_brightness;
};

void
free_devices(struct device** __devs);

int
get_all_device(struct device** __devs, char* __class_filter,
               char* __deviceid_filter);
void
print_all_device(struct device** __devs);

int
parse_arg_percent_to_brightness_value(struct device* __dev, char* __arg_str);

int
set_brightness(struct device* __dev, unsigned int __value);

unsigned int
get_brightness(struct device* __dev);

#endif  // !BRIGHTNESS_H
