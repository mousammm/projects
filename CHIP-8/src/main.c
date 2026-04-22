#include "sdl.h"
#include "chip8.h"
#include <stdio.h>

int main(int argc,char **argv)
{
    /* talking argument */ 
    if (argc < 2){
      fprintf(stderr, "Usage <path/to/rom>");
      return 1;
    } 

    /* initialize chip8 */
    chip8_t chip8 = {0};
    chip8_init(&chip8);

    /* loading rom */ 
    if (!chip8_load_rom(&chip8,argv[1])) return 1;

    // sdl initialisation
    sdl_t sdl = {0};
    if(!sdl_init(&sdl)) return 1;

    
    while (sdl_handle_inputs(&chip8)) {

       chip8_cycle(&chip8);     // chip8 one cycle
       chip8_timers(&chip8);    // update timers 
       
       if (chip8.draw_flag) {   // render frame 
          sdl_render_frame(&sdl,&chip8);
       }

       SDL_Delay(2);
     } 

    sdl_cleanup(&sdl);          // cleanup
    return 0;
}

