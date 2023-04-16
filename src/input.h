#ifndef KB_OVERLAY_INPUT_H
#define KB_OVERLAY_INPUT_H

#include <X11/Xlib.h>
#include <X11/extensions/XInput.h>

extern int xi_opcode;

void init(int mouse_id, int keyboard_id);

void lookup_x11_keycode(Display* display, int keycode, char* buffer);

#endif //KB_OVERLAY_INPUT_H
