#include <SDL2/SDL_render.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define PI 3.1415926535
#define P2 PI/2
#define P3 3*P2
#define DR  0.0174533 /* one degree in radian */

SDL_Window* window;
SDL_Renderer* renderer;
#define WIDTH 1024
#define HEIGHT 512

bool quit = false;
float px=100, py=100, /* initial player pos */
    vel=5,  /* player speed */
    pdx, pdy, /* player delta x snd y */
    pa; /* player angle */

int mapX=8, mapY=8, mapS=64;
int map[]={
    1,1,1,1,1,1,1,1,
    1,0,0,0,0,0,0,1,
    1,0,0,0,0,1,0,1,
    1,0,0,0,0,1,1,1,
    1,0,0,0,0,0,0,1,
    1,0,0,1,1,0,0,1,
    1,0,0,1,0,0,0,1,
    1,1,1,1,1,1,1,1,
};

void draw_map() {
    for (int y=0; y<mapY; y++) {
        for (int x=0; x<mapX; x++) {

            if (map[y*mapX+x]==1) { /* walls colors */
                SDL_SetRenderDrawColor(renderer, 200, 200, 200, 200); /* white */
            } else { /* empty spcae colors */
                SDL_SetRenderDrawColor(renderer, 100, 0, 100, 255); /* black */
            }

            /* x,y pos for all ele of map array */
            int xo=x*mapS;
            int yo=y*mapS;

            /* draw the squares */
            SDL_Rect rect = {xo, yo, mapS-1, mapS-1};
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

void draw_player() {
    /* player */
    SDL_Rect rect = {px, py, 10, 10};
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &rect);

    /* line showing angle */
    /* 10 the size of player /2 to get the ray occur from center */
    SDL_SetRenderDrawColor(renderer, 100, 0, 0, 255); /* black */
    SDL_RenderDrawLine(renderer, px+(10/2), py+(10/2), px+pdx*5, py+pdy*5);

}

/* return dist btw the player and ray endpoint */
float dist(float ax, float ay, float bx, float by, float ang) {
    return (sqrtf((bx-ax)*(bx-ax) + (by-ay)*(by-ay)));
}


/* dda algo */
void draw_ray() {
    /* mapX = 8; mapY=8 */
    int r, mx, my, mp, dof;
    float rx, ry, ra, xo, yo, disT;
    ra = pa-DR*360; /* rays angle = players angle */
    if (ra<0) {
        ra+=2*PI;
        if (ra>2*PI) {
            ra-=2*PI;
        }
    }


    for (r=0; r<360; r++) {
        /* check horizontal line */
        dof=0;
        float disH=100000,hx=px,hy=py;

        float aTan=-1/tan(ra);
        if (ra>PI) { /* ray loking down */
            ry = (((int)py>>6)<<6)-0.0001;
            rx=(py-ry)*aTan+px;
            yo=-64;
            xo=-yo*aTan;
        }
        if (ra<PI) { /* ray loking up */
            ry = (((int)py>>6)<<6)+64;
            rx=(py-ry)*aTan+px;
            yo=64;
            xo=-yo*aTan;
        }
        if (ra==0 || ra==PI) { /* ray looking straight left or right */
            rx=px; ry=py;
            dof=8;
        }
        while (dof<8) {
            mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*8+mx;
            if (mp>0 && mp<8*8 && map[mp]==1) { /* hit the wall */ 
                hx=rx;
                hy=ry;
                disH=dist(px, py, hx, hy, ra);
                dof=8;
            }
            else { /* next line */
                rx+=xo;
                ry+=yo;
                dof+=1;
            }
        }
        /* draw the horizontal line */
        // SDL_RenderDrawLine(renderer, px+(10/2), py+(10/2), rx, ry);

        /* check vertical line */
        dof=0;
        float disV = 100000,vx=px,vy=py;

        float nTan=-tan(ra);
        if (ra>P2 && ra<P3) { /* ray left */
            rx = (((int)px>>6)<<6)-0.0001;
            ry=(px-rx)*nTan+py;
            xo=-64;
            yo=-xo*nTan;
        }
        if (ra<P2 || ra>P3) { /* ray loking right */
            rx = (((int)px>>6)<<6)+64;
            ry=(px-rx)*nTan+py;
            xo=64;
            yo=-xo*nTan;
        }
        if (ra==0 || ra==PI) { /* ray looking straight up or down*/
            rx=px; ry=py;
            dof=8;
        }
        while (dof<8) {
            mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*8+mx;
            if (mp>0 && mp<8*8 && map[mp]==1) { /* hit the wall */ 
                vx=rx;
                vy=ry;
                disV=dist(px, py, vx, vy, ra);
                dof=8;
            }
            else { /* next line */
                rx+=xo;
                ry+=yo;
                dof+=1;
            }
        }

        /* take the shortest betw horizontal and vetical  */
        if (disV<disH) { rx=vx; ry=vy; disT=disV; }
        if (disH<disV) { rx=hx; ry=hy; disT=disH; }
        /* draw the horizontal line */
        SDL_RenderDrawLine(renderer, px+(10/2), py+(10/2), rx, ry);

        /* draw the walls */
        float lineH=(mapS*320)/disT;
        if (lineH>320) { lineH=320; }
        float lineO=160-lineH/2;
        SDL_RenderDrawLine(renderer, r*2+530, lineO, r*2+530, lineH+lineO);

        /* increase 1radian evrythime */
        ra+=DR;
        if (ra<0) {
            ra+=2*PI; if (ra>2*PI) { ra-=2*PI; }
        }

    }
}

void sdl_poll_event() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        switch (e.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYUP:
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                } // keysym.sym
                break;
            case SDL_KEYDOWN:
                switch (e.key.keysym.sym) {
                    case SDLK_w: /* up */
                        // py -= vel;
                        px+=pdx; py+=pdy;
                        break;
                    case SDLK_s: /* down */
                        // py += vel;
                        px-=pdx; py-=pdy;
                        break;
                    case SDLK_a: /* left */
                        // px -= vel;
                        pa-=0.1;
                        if (pa<0) { pa+=2*PI; }
                        pdx=cos(pa)*5;
                        pdy=sin(pa)*5;
                        break;
                    case SDLK_d: /* right */
                        // px += vel;
                        pa+=0.1;
                        if (pa>2*PI) { pa-=2*PI; }
                        pdx=cos(pa)*5;
                        pdy=sin(pa)*5;
                        break;
                } // keysym.sym
                break;
        
        } // e.type
    } // while 
}

int main(int argc, char **argv) {

    /* Sdl init */    
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Raycaster",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    /* main loooop */
    while (!quit) {
        sdl_poll_event(); /* handle events */

        /* clear once */
        /* dont need to do this in draw fun */
        SDL_SetRenderDrawColor(renderer, 0, 30, 30, 50);  /* background color */
        SDL_RenderClear(renderer);

        /* draw */
        draw_map();
        draw_player();
        draw_ray();

        /* push everything to screen at once */
        /* dont need to do this in draw fun */
        SDL_RenderPresent(renderer);

        SDL_Delay(10);/* 60fps */ 
    }

    /* Destroy */
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
}
