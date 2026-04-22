#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

/* SDL and window */
SDL_Window *pwindow;
SDL_Renderer *prenderer;

int cellSize=40; // pixels per grid cells
#define MAP_WIDTH 10
#define MAP_HEIGHT 10
#define S_HEIGHT cellSize*MAP_HEIGHT
#define S_WIDTH S_HEIGHT*2
bool quit = false;

/* map */
int worldMap[MAP_WIDTH][MAP_HEIGHT] = {
  {1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,1},
  {1,0,0,1,1,1,0,0,0,1},
  {1,0,0,0,0,1,0,0,0,1},
  {1,0,0,0,0,1,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1}
};

/* player */
float px=100, /* initial x pos */
      py=100, /* initial y pos */
      ps=5;   /* player size */
float pdirX=1.0, /* pointing right initially */
      pdirY=0.0,
      pplaneX=0.0,
      pplaneY=0.66; /* field of view */


void drawMap(){

  for(int i=0; i<MAP_WIDTH; i++){
    for(int j=0; j<MAP_HEIGHT; j++){
      if (worldMap[i][j] > 0) { /* Wall */
          SDL_Rect cell = {i * cellSize, j * cellSize, cellSize-1, cellSize-1};
          SDL_SetRenderDrawColor(prenderer, 100, 100, 100, 255);
          SDL_RenderFillRect(prenderer, &cell);
      } else { /* empty space */
          SDL_Rect cell = {i * cellSize, j * cellSize, cellSize-1, cellSize-1};
          SDL_SetRenderDrawColor(prenderer, 50, 50, 50, 255);
          SDL_RenderFillRect(prenderer, &cell);
      }
    }
  }

}

void gameControls(){
  SDL_Event e;
  while (SDL_PollEvent(&e) != 0) {
    switch (e.type) {
      case SDL_QUIT:
        quit = true;
        break;
      case SDL_KEYDOWN:
        switch (e.key.keysym.sym) {
          case SDLK_w: /* up */
              py -= 10;
              break;
          case SDLK_s: /* down */
              py += 10;
              break;
          case SDLK_a: /* left */
              px -= 10;
              break;
          case SDLK_d: /* right */
              px += 10;
              break;
          break;
      } /* e.key.keysym.sym */
      break;
    } /* switch e.type */
  }
}

int main() {
  /* Init SDL */
  SDL_Init(SDL_INIT_VIDEO);
  pwindow = SDL_CreateWindow("raycaster",
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      S_WIDTH, S_HEIGHT, SDL_WINDOW_SHOWN);
  prenderer = SDL_CreateRenderer(pwindow, -1, SDL_RENDERER_ACCELERATED);

  /* game loop */
  while(!quit) {
    /* handle input */
    gameControls();

    /* set cur color + clear with the color */
    SDL_SetRenderDrawColor(prenderer, 0, 30, 30, 50);
    SDL_RenderClear(prenderer);

    /* draw stuff */
    drawMap(); /* map */

    /* player */
    SDL_Rect player={px,py,ps,ps};
    SDL_SetRenderDrawColor(prenderer, 255, 0, 0, 255);
    SDL_RenderFillRect(prenderer, &player);


    /* update the screen */
    SDL_RenderPresent(prenderer);
  }

  /* destroy */
  SDL_DestroyRenderer(prenderer);
  SDL_DestroyWindow(pwindow);
  SDL_Quit();

  return 0;
}

