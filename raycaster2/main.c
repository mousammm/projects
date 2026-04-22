#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

SDL_Window *pwindow;
SDL_Renderer *prenderer;
#define WIDTH 640
#define HEIGHT 480

bool quit = false;

int main() {
  /* Init SDL */
  SDL_Init(SDL_INIT_VIDEO);
  pwindow = SDL_CreateWindow("raycaster",
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
  prenderer = SDL_CreateRenderer(pwindow, -1, SDL_RENDERER_ACCELERATED);

  /* game loop */
  while(!quit) {
    /* handle input */
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        switch (e.type) {
            case SDL_QUIT:
                quit = true;
                break;
        }
    }

    /* set cur color + clear with the color */
    SDL_SetRenderDrawColor(prenderer, 0, 30, 30, 50);
    SDL_RenderClear(prenderer);

    /* draw stuff */

    /* update the screen */
    SDL_RenderPresent(prenderer);
  }

  /* destroy */
  SDL_DestroyRenderer(prenderer);
  SDL_DestroyWindow(pwindow);
  SDL_Quit();

  return 0;
}
