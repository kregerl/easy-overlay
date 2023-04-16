#ifndef KB_OVERLAY_RENDERER_H
#define KB_OVERLAY_RENDERER_H
#include <stdbool.h>
#include <X11/Xlib.h>

#include <SDL2/SDL_render.h>

void render_keys(Display* display, SDL_Renderer* renderer, const bool held_keys[], const bool pressed_buttons[]);

#endif //KB_OVERLAY_RENDERER_H
