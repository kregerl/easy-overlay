#include "input.h"
#include "renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/XKBlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define INVALID_EVENT_TYPE -1
static int key_press_type = INVALID_EVENT_TYPE;
static int key_release_type = INVALID_EVENT_TYPE;

static int button_press_type = INVALID_EVENT_TYPE;
static int button_release_type = INVALID_EVENT_TYPE;

int xi_opcode;

int register_events(Display* display, XDeviceInfo device_info) {
    unsigned long screen = DefaultScreen(display);
    Window root_window = RootWindow(display, screen);
    XDevice* device = XOpenDevice(display, device_info.id);
    if (!device) {
        fprintf(stderr, "Unable to open device '%lu'\n", device_info.id);
        exit(EXIT_FAILURE);
    }
    // Where the event types will be held
    // Only listening to KeyPress and KeyRelease so there is 2 events in the list.
    int num_events = 0;
    XEventClass event_list[2];
    if (device->num_classes > 0) {
        for (int i = 0; i < device_info.num_classes; i++) {
            XInputClassInfo class_info = device->classes[i];
            switch (class_info.input_class) {
                case KeyClass: {
                    DeviceKeyPress(device, key_press_type, event_list[num_events])
                    num_events++;
                    DeviceKeyRelease(device, key_release_type, event_list[num_events])
                    num_events++;
                    break;
                }
                case ButtonClass: {
                    printf("ButtonClass\n");
                    DeviceButtonPress(device, button_press_type, event_list[num_events])
                    num_events++;
                    DeviceButtonRelease(device, button_release_type, event_list[num_events])
                    num_events++;
                    break;
                }
                default: {
                    break;
                }
            }
        }
        // Validate events are registered
        if (XSelectExtensionEvent(display, root_window, event_list, num_events)) {
            fprintf(stderr, "error selecting extended events\n");
            exit(EXIT_FAILURE);
        }
    }
    return num_events;
}

void start_event_listener(Display* display, SDL_Renderer* renderer) {
    XEvent event;
    setvbuf(stdout, NULL, _IOLBF, 0);
    bool held_keys[255];
    bool pressed_buttons[3];
    memset(&held_keys, 0, 255);
    while (1) {
        XNextEvent(display, &event);
        if (event.type == key_press_type || event.type == key_release_type) {
            XDeviceKeyEvent* key = (XDeviceKeyEvent*) &event;
            if (event.type == key_press_type) {
                held_keys[key->keycode] = true;
            } else {
                held_keys[key->keycode] = false;
            }
        } else if (event.type == button_press_type || event.type == button_release_type) {
            XDeviceButtonEvent* button = (XDeviceButtonEvent*) &event;
            if (event.type == button_press_type)
                pressed_buttons[button->button - 1] = true;
            else
                pressed_buttons[button->button - 1] = false;
        }

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderClear(renderer);
        render_keys(display, renderer, held_keys, pressed_buttons);

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:goto EXIT_LOOP;
            }
        }
        SDL_RenderPresent(renderer);

    }
    EXIT_LOOP:
    return;
}

XDeviceInfo prompt_device_selection(Display* display) {
    XDeviceInfo* devices;
    int num_devices;
    devices = XListInputDevices(display, &num_devices);

    // Print list of devices for user to select from
    for (int i = 0; i < num_devices; i++) {
        XDeviceInfo info = devices[i];
        printf("%d: %s(%lu)\n", i + 1, info.name, info.id);
    }
    printf("Select which device to use by number: ");
    char id_buffer[4];
    scanf("%4s", id_buffer);

    // Convert user's input to int and index device array.
    int id = (int) strtol((const char*) &id_buffer, NULL, 10);
    if (!id || id > num_devices || id < 1) {
        fprintf(stderr, "'%d' is not a valid device id.\n", id);
        exit(EXIT_FAILURE);
    }
    // Return selected device
    return devices[id - 1];
}

XDeviceInfo get_device_info(Display* display, int id) {
    int num_devices;
    XDeviceInfo* devices = XListInputDevices(display, &num_devices);

    if (!id || id > num_devices || id < 1) {
        fprintf(stderr, "'%d' is not a valid device id.\n", id);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < num_devices; i++) {
        XDeviceInfo info = devices[i];
        if (info.id == id) {
            return devices[i];
        }
    }
    fprintf(stderr, "No device with id '%d'\n", id);
    exit(EXIT_FAILURE);
}

void lookup_x11_keycode(Display* display, int keycode, char buffer[32]) {
    XKeyPressedEvent event;
    event.type = KeyPress;
    event.display = display;
    event.state = 0;
    event.keycode = keycode;
    KeySym ignore;
    Status return_status;
    XIM xim = XOpenIM(display, 0, 0, 0);
    XIC xic = XCreateIC(xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, NULL);

    Xutf8LookupString(xic, &event, buffer, 32, &ignore, &return_status);
}

void init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Could not initialize SDL2: %s\n", SDL_GetError());
        return;
    }

    if (TTF_Init() < 0) {
        fprintf(stderr, "Could not initialize SDL_TTF: %s\n", TTF_GetError());
        return;
    }

    SDL_Window* window = SDL_CreateWindow("Keyboard overlay",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          700, 300,
                                          SDL_WINDOW_SHOWN);

    if (!window) {
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        return;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer) {
        SDL_DestroyWindow(window);
        fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        return;
    }

    Display* display = XOpenDisplay(NULL);

    if (!display) {
        fprintf(stderr, "Unable to connect to X server\n");
        exit(EXIT_FAILURE);
    }

    int event, error;
    if (!XQueryExtension(display, "XInputExtension", &xi_opcode, &event, &error)) {
        fprintf(stderr, "XInput extension not available.\n");
        exit(EXIT_FAILURE);
    }

    // Prompt user to select device
    int id = 10;
    XDeviceInfo info;
    if (id < 0) {
        info = prompt_device_selection(display);
    } else {
        info = get_device_info(display, id);
    }

    int mouse_id = 13;
    XDeviceInfo mouse_info;
    if (id < 0) {
        mouse_info = prompt_device_selection(display);
    } else {
        mouse_info = get_device_info(display, mouse_id);
    }

    if (!register_events(display, mouse_info)) {
        fprintf(stderr, "No mouse events registered.\n");
        exit(EXIT_FAILURE);
    }

    XkbSetDetectableAutoRepeat(display, True, NULL);
    if (register_events(display, info)) {
        start_event_listener(display, renderer);
    } else {
        fprintf(stderr, "No keyboard events registered.\n");
        exit(EXIT_FAILURE);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
}

