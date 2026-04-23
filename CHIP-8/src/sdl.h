#pragma once

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "chip8.h"

#define SCALE_WINDOW 20

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
} sdl_t;

// function declaration
bool sdl_init(sdl_t *sdl);
void sdl_cleanup(sdl_t *sdl);
void sdl_render_frame(sdl_t *sdl,chip8_t *chip8);
bool sdl_handle_inputs(chip8_t *chip8);
