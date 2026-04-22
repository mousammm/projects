#include "sdl.h"

bool sdl_init(sdl_t *sdl)
{
    // Initialize SDL with video subsystem
    if ((SDL_Init(SDL_INIT_VIDEO) !=0 )) return false; 

    // Create window
    sdl->window = SDL_CreateWindow(
        "Chip8",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCALE_WINDOW*DISPLAY_WIDTH,
        SCALE_WINDOW*DISPLAY_HEIGHT,
        SDL_WINDOW_SHOWN);
    if (!sdl->window) return false;
    
    // Create renderer 
    sdl->renderer = SDL_CreateRenderer(sdl->window, -1, 0);
    if (!sdl->renderer) return false; 
    
    return true;
}

void sdl_cleanup(sdl_t *sdl)
{
    if(sdl->window) SDL_DestroyWindow(sdl->window);
    if(sdl->renderer) SDL_DestroyRenderer(sdl->renderer);
    SDL_Quit();
}

void sdl_render_frame(sdl_t *sdl, chip8_t *chip8)
{
    // Clear screen with black background
    SDL_SetRenderDrawColor(sdl->renderer, 0, 0, 0, 255);    // RGBA: Black
    SDL_RenderClear(sdl->renderer);                         // Fill entire renderer

    // Set pixel color to white
    SDL_SetRenderDrawColor(sdl->renderer, 255, 255, 255, 255);  // RGBA: White
    
    // Draw CHIP-8 display pixels
    for (int y = 0; y < DISPLAY_HEIGHT; y++)
    {
        for (int x = 0; x < DISPLAY_WIDTH; x++) 
        {
            // Check if pixel is ON (non-zero) in CHIP-8 display buffer
            if (chip8->display[y * DISPLAY_WIDTH + x]) 
            {
                // Create scaled rectangle for pixel
                SDL_Rect pixel = {
                    x * SCALE_WINDOW,      // X position (scaled)
                    y * SCALE_WINDOW,      // Y position (scaled)
                    SCALE_WINDOW,          // Width (20 pixels)
                    SCALE_WINDOW           // Height (20 pixels)
                };
                // Fill the rectangle
                SDL_RenderFillRect(sdl->renderer, &pixel);
            }
        }
    }

    // Update display
    SDL_RenderPresent(sdl->renderer);  // Show rendered frame
    chip8->draw_flag = false;
}

bool sdl_handle_inputs(chip8_t *chip8)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT: return false; 
                
            case SDL_KEYDOWN:
            case SDL_KEYUP:
            {
                bool pressed = (event.type == SDL_KEYDOWN);
                uint8_t key = 0xFF;
                
                switch(event.key.keysym.sym)
                {
                    case SDLK_1: key = 0x1; break;
                    case SDLK_2: key = 0x2; break;
                    case SDLK_3: key = 0x3; break;
                    case SDLK_4: key = 0xC; break;
                    case SDLK_q: key = 0x4; break;
                    case SDLK_w: key = 0x5; break;
                    case SDLK_e: key = 0x6; break;
                    case SDLK_r: key = 0xD; break;
                    case SDLK_a: key = 0x7; break;
                    case SDLK_s: key = 0x8; break;
                    case SDLK_d: key = 0x9; break;
                    case SDLK_f: key = 0xE; break;
                    case SDLK_z: key = 0xA; break;
                    case SDLK_x: key = 0x0; break;
                    case SDLK_c: key = 0xB; break;
                    case SDLK_v: key = 0xF; break;
                    case SDLK_ESCAPE: 
                        if (pressed) return false;
                    break;
                }
                
                // Update CHIP-8 keypad state
                if(key < 16) {
                    chip8->keypad[key] = pressed;
                }
                break;
            } 
        } 
    } 
    return true;
}
