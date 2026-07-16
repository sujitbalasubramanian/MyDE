#ifndef XDG_STUB_H
#define XDG_STUB_H

#include <wayland-util.h>

// Stubbing the xdg-shell interfaces wlr-layer-shell references.
// By zero-initializing them, we give the linker the exact symbols it wants.
const struct wl_interface xdg_popup_interface = {0};

// You will almost certainly need this one too, as layer-shell
// ties into standard xdg_surfaces in its XML.
const struct wl_interface xdg_surface_interface = {0};

#endif  // !XDG_STUB_H
