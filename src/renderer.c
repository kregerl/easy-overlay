#include <SDL2/SDL_ttf.h>
#include "renderer.h"
#include "input.h"

#define SIZE 50

void set_render_color(SDL_Renderer* renderer, bool is_enabled) {
    if (is_enabled) {
        SDL_SetRenderDrawColor(renderer, 156, 156, 156, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    }
}

void render_text(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Point point) {
    SDL_Color color = {
            .r = 255,
            .g = 255,
            .b = 255,
            .a = 255,
    };
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect;
    rect.x = point.x;
    rect.y = point.y;
    rect.w = surface->w;
    rect.h = surface->h;

    SDL_RenderCopy(renderer, texture, NULL, &rect);
}


void render_tilde_row(Display* display, SDL_Renderer* renderer, TTF_Font* font, const bool held_keys[]) {
    const int offset = 75;
    for (int i = 0; i < 5; i++) {
        int keycode = 10 + i;
        set_render_color(renderer, held_keys[keycode]);
        SDL_Rect rect = {
                .x = offset + 20 + (SIZE * i) + (i * 5),
                .y = 20,
                .w = SIZE,
                .h = SIZE
        };
        SDL_RenderFillRect(renderer, &rect);
        SDL_Point point = {.x = rect.x + 15, .y = rect.y + 10};
        char buffer[32];
        lookup_x11_keycode(display, keycode, buffer);
        render_text(renderer, font, buffer, point);
    }
}

void render_tab_row(Display* display, SDL_Renderer* renderer, TTF_Font* font, const bool held_keys[]) {
    int offset = 0;
    for (int i = 0; i < 6; i++) {
        int keycode = 23 + i;
        set_render_color(renderer, held_keys[keycode]);

        int w = SIZE;
        if (i == 0)
            w += SIZE;
        SDL_Rect rect = {
                .x = offset + 20 + (SIZE * i) + (i * 5),
                .y = 20 + SIZE + 5,
                .w = w,
                .h = SIZE
        };
        SDL_RenderFillRect(renderer, &rect);
        SDL_Point point = {.x = rect.x + 15, .y = rect.y + 10};
        if (i == 0) {
            render_text(renderer, font, "tab", point);
        } else {
            char buffer[32];
            lookup_x11_keycode(display, keycode, buffer);
            render_text(renderer, font, buffer, point);
        }
        if (i == 0)
            offset += SIZE;
    }
}

void render_capslock_row(Display* display, SDL_Renderer* renderer, TTF_Font* font, const bool held_keys[]) {
    set_render_color(renderer, held_keys[66]);
    SDL_Rect caps_rect = {
            .x = 20,
            .y = 20 + (SIZE * 2) + 10,
            .w = (SIZE * 2 + 20),
            .h = SIZE
    };
    SDL_RenderFillRect(renderer, &caps_rect);
    SDL_Point ctrl_point = {.x = caps_rect.x + 10, .y = caps_rect.y + 10};
    render_text(renderer, font, "caps", ctrl_point);

    int offset = SIZE * 2 + 25;
    for (int i = 0; i < 5; i++) {
        int keycode = 38 + i;
        set_render_color(renderer, held_keys[keycode]);
        SDL_Rect rect = {
                .x = offset + 20 + (SIZE * i) + (i * 5),
                .y = 20 + (SIZE * 2) + 10,
                .w = SIZE,
                .h = SIZE
        };
        SDL_RenderFillRect(renderer, &rect);
        SDL_Point point = {.x = rect.x + 15, .y = rect.y + 10};
        char buffer[32];
        lookup_x11_keycode(display, keycode, buffer);
        render_text(renderer, font, buffer, point);
    }
}

void render_shift_row(Display* display, SDL_Renderer* renderer, TTF_Font* font, const bool held_keys[]) {
    set_render_color(renderer, held_keys[50]);
    SDL_Rect shift_rect = {
            .x = 20,
            .y = 20 + (SIZE * 3) + 15,
            .w = (SIZE * 2 + 40),
            .h = SIZE
    };
    SDL_RenderFillRect(renderer, &shift_rect);
    SDL_Point ctrl_point = {.x = shift_rect.x + 10, .y = shift_rect.y + 10};
    render_text(renderer, font, "shift", ctrl_point);


    int offset = (SIZE * 2 + 45);
    for (int i = 0; i < 4; i++) {
        int keycode = 52 + i;
        set_render_color(renderer, held_keys[keycode]);
        SDL_Rect rect = {
                .x = offset + 20 + (SIZE * i) + (i * 5),
                .y = 20 + (SIZE * 3) + 15,
                .w = SIZE,
                .h = SIZE
        };
        SDL_RenderFillRect(renderer, &rect);
        SDL_Point point = {.x = rect.x + 15, .y = rect.y + 10};
        char buffer[32];
        lookup_x11_keycode(display, keycode, buffer);
        render_text(renderer, font, buffer, point);
    }
}

void render_ctrl_row(SDL_Renderer* renderer, TTF_Font* font, const bool held_keys[]) {
    // Ctrl
    set_render_color(renderer, held_keys[37]);
    SDL_Rect ctrl_rect = {
            .x = 20,
            .y = 20 + (SIZE * 4) + 20,
            .w = SIZE + 25,
            .h = SIZE
    };
    SDL_RenderFillRect(renderer, &ctrl_rect);
    SDL_Point ctrl_point = {.x = ctrl_rect.x + 10, .y = ctrl_rect.y + 10};
    render_text(renderer, font, "ctrl", ctrl_point);

    // Alt
    set_render_color(renderer, held_keys[64]);
    SDL_Rect alt_rect = {
            .x = 20 + (SIZE * 2) + 25,
            .y = 20 + (SIZE * 4) + 20,
            .w = SIZE + 25,
            .h = SIZE
    };
    SDL_RenderFillRect(renderer, &alt_rect);
    SDL_Point alt_point = {.x = alt_rect.x + 20, .y = alt_rect.y + 10};
    render_text(renderer, font, "alt", alt_point);

    // Space
    set_render_color(renderer, held_keys[65]);
    SDL_Rect space_rect = {
            .x = 20 + (SIZE * 4) + 5,
            .y = 20 + (SIZE * 4) + 20,
            .w = 235,
            .h = SIZE
    };
    SDL_RenderFillRect(renderer, &space_rect);
    SDL_Point space_point = {.x = space_rect.x + (space_rect.w / 3), .y = space_rect.y + 10};
    render_text(renderer, font, "space", space_point);
}

void render_buttons(SDL_Renderer* renderer, const bool pressed_buttons[]) {
    // LMB
    set_render_color(renderer, pressed_buttons[0]);
    SDL_Rect lmb_rect = {
            .x = 500,
            .y = 20,
            .w = 80,
            .h = 200
    };
    SDL_RenderFillRect(renderer, &lmb_rect);

    // MMB
    set_render_color(renderer, pressed_buttons[1]);
    SDL_Rect mmb_rect = {
            .x = 580,
            .y = 20,
            .w = 20,
            .h = 100
    };
    SDL_RenderFillRect(renderer, &mmb_rect);

    // Filler
    set_render_color(renderer, false);
    SDL_Rect filler_rect = {
            .x = 580,
            .y = 120,
            .w = 20,
            .h = 100
    };
    SDL_RenderFillRect(renderer, &filler_rect);

    // RMB
    set_render_color(renderer, pressed_buttons[2]);
    SDL_Rect rmb_rect = {
            .x = 600,
            .y = 20,
            .w = 80,
            .h = 200
    };
    SDL_RenderFillRect(renderer, &rmb_rect);

}


void render_keys(Display* display, SDL_Renderer* renderer, const bool held_keys[], const bool pressed_buttons[]) {
    TTF_Font* font = TTF_OpenFont("OpenSans-Regular.ttf", 24);

    if (!font) {
        fprintf(stderr, "Could not open font\n");
        exit(1);
    }

    // This sucks but it'll do
    render_tilde_row(display, renderer, font, held_keys);
    render_tab_row(display, renderer, font, held_keys);
    render_capslock_row(display, renderer, font, held_keys);
    render_shift_row(display, renderer, font, held_keys);
    render_ctrl_row(renderer, font, held_keys);
    render_buttons(renderer, pressed_buttons);
}