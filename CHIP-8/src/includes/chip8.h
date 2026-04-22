#pragma once

#include <stdint.h> 
#include <stdbool.h> 

#define MEMORY 4096
#define REGISTER 16
#define STACK 16
#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define NUM_PAD 16

//chip8 define
typedef struct {
  uint8_t memory[MEMORY];                         // 4kb ram/memory
  uint8_t V[REGISTER];                            // 16 8-bit register VF=carry flag
  uint16_t I;                                     // Index register 
  uint16_t pc;                                    // Program counter
  uint16_t stack[STACK];                          // stack
  uint8_t sp;                                     // stack pointer
  uint8_t delay_timer;                            // delay timer
  uint8_t sound_timer;                            // sound timer
  uint8_t display[DISPLAY_WIDTH*DISPLAY_HEIGHT];  // 64*32 pixel display
  bool keypad[NUM_PAD];                           // keypad state [0-F]
  bool draw_flag;                                 // redraw flag 
} chip8_t;

//chip8 function declaration 
void chip8_init(chip8_t *chip8);
bool chip8_load_rom(chip8_t *chip8, const char *filename);
void chip8_cycle(chip8_t *chip8);
void chip8_timers(chip8_t *chip8);
