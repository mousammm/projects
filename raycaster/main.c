#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define S_WIDTH  640
#define S_HEIGHT 480
/* SDL and window */
SDL_Window *pwindow;
SDL_Renderer *prenderer;
bool quit = false;

/* world grid: 0-10 in X and Y */
/* each cell is exactly 1.0 unit */
#define MAP_WIDTH 10
#define MAP_HEIGHT 10
#define CELL_SIZE 10 /* Only for drawing */
int worldMap[MAP_WIDTH][MAP_HEIGHT] = {
  {1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,1},
  {1,2,2,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,1},
  {1,0,0,1,1,1,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1}
};

/* Player in world units 0-MAP_WIDTH */
float px=5.0, py=5.0, ps=0.2;   /* player size for drawing */
/* cmera */
float pdirX=1.0, pdirY=0.0;    /* where player is looking */
float pplaneX=0.0, pplaneY=0.66; /* camera fov */
/* Movement speed - in world units */
float moveSpeed = 0.05, rotSpeed = 0.05; // 0.03 ~ 1.7 ded per frame

void drawMap(){
  for(int y=0; y<MAP_HEIGHT; y++){ /* X */
    for(int x=0; x<MAP_WIDTH; x++){ /* Y */
      if (worldMap[x][y] > 0) { /* Wall */
          SDL_Rect cell = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
          SDL_SetRenderDrawColor(prenderer, 100, 100, 100, 255);
          SDL_RenderFillRect(prenderer, &cell);
      } else { /* empty space */
          SDL_Rect cell = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
          SDL_SetRenderDrawColor(prenderer, 200, 150, 100, 255);
          SDL_RenderFillRect(prenderer, &cell);
      }
    }
  }
}

void castRays() {
    // Loop through every vertical line(column) of the screen
    for (int x = 0; x < S_WIDTH; x++) {
        // 1. Calculate ray direction for this column
        float cameraX = 2.0 * x / S_WIDTH - 1.0;  // -1 (left) to +1 (right)
        float rayDirX = pdirX + pplaneX * cameraX;
        float rayDirY = pdirY + pplaneY * cameraX;

        // 2. Which grid cell the ray starts in
        int mapX = (int)px;
        int mapY = (int)py;

        // 3. Delta distances (how far to travel to cross a grid line)
        float deltaDistX = fabs(1.0 / rayDirX);
        float deltaDistY = fabs(1.0 / rayDirY);

        // 4. Step direction (+1 or -1)
        int stepX = (rayDirX < 0) ? -1 : 1;
        int stepY = (rayDirY < 0) ? -1 : 1;

        // 5. Calculate initial side distances
        float sideDistX, sideDistY;

        if (rayDirX < 0)
            sideDistX = (px - mapX) * deltaDistX;
        else
            sideDistX = (mapX + 1.0 - px) * deltaDistX;

        if (rayDirY < 0)
            sideDistY = (py - mapY) * deltaDistY;
        else
            sideDistY = (mapY + 1.0 - py) * deltaDistY;

        // 6. DDA: Walk through grid until hitting a wall
        int hit = 0;
        int side = 0;  // 0 = vertical wall, 1 = horizontal wall

        while (!hit) {
            if (sideDistX < sideDistY) {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            } else {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }
            if (worldMap[mapX][mapY] > 0) hit = 1;
        }

        // 7. Calculate perpendicular distance (no fish-eye)
        float perpWallDist;
        if (side == 0)
            //perpWallDist = (sideDistX - deltaDistX);
            perpWallDist = (sideDistX - deltaDistX);
        else
            //perpWallDist = (sideDistY - deltaDistY);
            perpWallDist = (sideDistY - deltaDistY);

        //float fishEyeDist = perpWallDist / sqrtf(1.0 + cameraX * cameraX);
        float fishEyeDist = perpWallDist / cos(atan(cameraX * 0.66));
        
        // 8. Calculate wall height and drawing limits
        //int lineHeight = (int)(S_HEIGHT / perpWallDist);
        int lineHeight = (int)(S_HEIGHT / fishEyeDist);
        int drawStart = -lineHeight / 2 + S_HEIGHT / 2;
        if (drawStart < 0) drawStart = 0;
        int drawEnd = lineHeight / 2 + S_HEIGHT / 2;
        if (drawEnd >= S_HEIGHT) drawEnd = S_HEIGHT - 1;

        // 9. Choose color based on wall type
        SDL_Color color;
        if (worldMap[mapX][mapY] == 1) {
            color = (SDL_Color){255, 0, 0, 255};  // Red wall
        } else {
            color = (SDL_Color){0, 255, 0, 255};  // Green wall
        }

        // Darken horizontal walls (side == 1) for shading
        if (side == 1) {
            color.r /= 2;
            color.g /= 2;
            color.b /= 2;
        }

        // 10. Draw the vertical line (the wall slice)
        SDL_SetRenderDrawColor(prenderer, color.r, color.g, color.b, 255);
        SDL_RenderDrawLine(prenderer, x, drawStart, x, drawEnd);
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
            {
            //px += pdirX * moveSpeed;
            //py += pdirY * moveSpeed;
            /* collitsion detc */
            float newX = px + pdirX * moveSpeed;
            float newY = py + pdirY * moveSpeed;
            if (worldMap[(int)newX][(int)py] == 0) px = newX;
            if (worldMap[(int)px][(int)newY] == 0) py = newY;
            }
            break;
          case SDLK_s: /* down */
            {
            //px -= pdirX * moveSpeed;
            //py -= pdirY * moveSpeed;
            /* collitsion detc */
            float newX = px - pdirX * moveSpeed;
            float newY = py - pdirY * moveSpeed;
            if (worldMap[(int)newX][(int)py] == 0) px = newX;
            if (worldMap[(int)px][(int)newY] == 0) py = newY;
            }
            break;
          case SDLK_d: /* left */
            {
            float oldDirX = pdirX;
            pdirX = pdirX * cos(rotSpeed) - pdirY * sin(rotSpeed);
            pdirY = oldDirX * sin(rotSpeed) + pdirY * cos(rotSpeed);

            float oldPlaneX = pplaneX;
            pplaneX = pplaneX * cos(rotSpeed) - pplaneY * sin(rotSpeed);
            pplaneY = oldPlaneX * sin(rotSpeed) + pplaneY * cos(rotSpeed);
            }
            break;
          case SDLK_a: /* right */
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
    SDL_SetRenderDrawColor(prenderer, 0, 0, 0, 50);
    SDL_RenderClear(prenderer);

    /* dda rays */
    castRays();

    /* draw stuff */
    //drawMap(); /* map */

    /* player */
    /*
     SDL_Rect player={
      (int)(px * CELL_SIZE - ps * CELL_SIZE/2),  // World units to pixel
      (int)(py * CELL_SIZE - ps * CELL_SIZE/2),
      (int)(ps * CELL_SIZE),
      (int)(ps * CELL_SIZE)
    };
    SDL_SetRenderDrawColor(prenderer, 255, 0, 0, 255);
    SDL_RenderFillRect(prenderer, &player);
    */

    /* update the screen */
    SDL_RenderPresent(prenderer);
  }

  /* destroy */
  SDL_DestroyRenderer(prenderer);
  SDL_DestroyWindow(pwindow);
  SDL_Quit();

  return 0;
}

