#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define MAP_WIDTH 10
#define MAP_HEIGHT 10
#define CELL_SIZE 50 /* Only for drawing */
#define S_HEIGHT CELL_SIZE*MAP_HEIGHT
#define S_WIDTH S_HEIGHT*2

/* SDL and window */
SDL_Window *pwindow;
SDL_Renderer *prenderer;
bool quit = false;

/* world grid: 0-10 in X and Y */
/* each cell is exactly 1.0 unit */
/* map */
int worldMap[MAP_WIDTH][MAP_HEIGHT] = {
  {1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,1,0,0,0,1},
  {1,0,0,0,0,1,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,1},
  {1,0,1,0,0,0,0,0,0,1},
  {1,0,1,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1}
};

/* Player in world units 0-MAP_WIDTH */
float px=5.0, py=5.0, ps=0.2;   /* player size for drawing */

float pdirX=1.0, pdirY=0.0;    /* where player is looking */
float pplaneX=0.0, pplaneY=0.66; /* camera fov */

/* Movement speed - in world units */
float moveSpeed = 0.05, rotSpeed = 0.03; // 0.03 ~ 1.7 ded per frame


void drawMap(){
  for(int y=0; y<MAP_HEIGHT; y++){ /* Height Y axis */
    for(int x=0; x<MAP_WIDTH; x++){ /* Width X axix */
      if (worldMap[x][y] > 0) { /* Wall */
          SDL_Rect cell = {y * CELL_SIZE, x * CELL_SIZE, CELL_SIZE-1, CELL_SIZE-1};
          SDL_SetRenderDrawColor(prenderer, 100, 100, 100, 255);
          SDL_RenderFillRect(prenderer, &cell);
      } else { /* empty space */
          SDL_Rect cell = {y * CELL_SIZE, x * CELL_SIZE, CELL_SIZE-1, CELL_SIZE-1};
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
            px += pdirX * moveSpeed;
            py += pdirY * moveSpeed;
            break;
          case SDLK_s: /* down */
            px -= pdirX * moveSpeed;
            py -= pdirY * moveSpeed;
            break;
          case SDLK_a: /* left */
            {
            float oldDirX = pdirX;
            pdirX = pdirX * cos(rotSpeed) - pdirY * sin(rotSpeed);
            pdirY = oldDirX * sin(rotSpeed) + pdirY * cos(rotSpeed);

            float oldPlaneX = pplaneX;
            pplaneX = pplaneX * cos(rotSpeed) - pplaneY * sin(rotSpeed);
            pplaneY = oldPlaneX * sin(rotSpeed) + pplaneY * cos(rotSpeed);
            }
            break;
          case SDLK_d: /* right */
            {
            float oldDirX = pdirX;
            pdirX = pdirX * cos(-rotSpeed) - pdirY * sin(-rotSpeed);
            pdirY = oldDirX * sin(-rotSpeed) + pdirY * cos(-rotSpeed);

            float oldPlaneX = pplaneX;
            pplaneX = pplaneX * cos(-rotSpeed) - pplaneY * sin(-rotSpeed);
            pplaneY = oldPlaneX * sin(-rotSpeed) + pplaneY * cos(-rotSpeed);
            }
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
    SDL_Rect player={
      (int)(px * CELL_SIZE - ps * CELL_SIZE/2),  // World units to pixel
      (int)(py * CELL_SIZE - ps * CELL_SIZE/2),
      (int)(ps * CELL_SIZE),
      (int)(ps * CELL_SIZE)
    };
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

