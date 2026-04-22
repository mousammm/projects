#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h> 
#include <time.h> 
#include <string.h>
#include "chip8.h"

// chip8 fontset 
const uint8_t fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void chip8_init(chip8_t *chip8)
{
    memset(chip8,0,sizeof(chip8_t));

    // load font 
    memcpy(&chip8->memory[0],fontset,sizeof(fontset));

    // set program counter 
    chip8->pc = 0x200;
    chip8->draw_flag = true;

    // Seed random number generator
    srand(time(NULL));
}

bool chip8_load_rom(chip8_t *chip8,const char *filename)
{
    // open file 
    FILE *file = fopen(filename, "rb");
    if(!file) {
        fprintf(stderr, "Cannot open %s file\n", filename);
        return false;
    }

    fseek(file,0,SEEK_END);
    long size = ftell(file);
    rewind(file);

    // Add size validation
    if (size > (MEMORY - 0x200)) {
        fprintf(stderr, "ROM too large: %ld bytes\n", size);
        fclose(file);
        return false;
    }

    // read into the memory
    if (!fread(&chip8->memory[0x200],1,size,file)) return false;

    fclose(file);
    return true; 
}

void chip8_timers(chip8_t *chip8)
{
   if (chip8->delay_timer > 0) { 
      chip8->delay_timer--;
   }
   if (chip8->sound_timer > 0) { 
      if (chip8->sound_timer == 1) printf("Beep!\n");
      chip8->sound_timer--;
   }
}

void chip8_cycle(chip8_t *chip8)
{
   uint16_t cur_pc = chip8->pc;  // Address where this opcode was fetched
   uint16_t opcode = (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc+1];
   chip8->pc += 2;

   printf("\n PC:[0x%04X] OP:0x%04X ASM:", cur_pc, opcode);

   uint8_t first_nibble = (opcode & 0xF000) >> 12;  // 1111 0000 0000 0000 
   uint8_t x = (opcode & 0x0F00) >> 8;              // 0000 1111 0000 0000
   uint8_t y = (opcode & 0x00F0) >> 4;              // 0000 0000 1111 0000
   uint8_t n = opcode & 0x000F;                     // 0000 0000 0000 1111
   uint8_t kk = opcode & 0x00FF;                    // 0000 0000 1111 1111
   uint16_t nnn = opcode & 0x0FFF;                  // 0000 1111 1111 1111
   // opcode                                        // 1111 1111 1111 1111

   switch (first_nibble) {
      case 0x0:
         if (kk == 0xE0) { // Clear Screen 
            printf("CLS (Clear Screen)");
            memset(chip8->display, 0, sizeof(chip8->display));
            chip8->draw_flag = true;
            break;
         } else if (kk == 0xEE) { // Return from subroutine
            printf("RET (Return from subroutine)");
            chip8->sp--;
            chip8->pc = chip8->stack[chip8->sp];
            break;
         } else {
             printf("SYS 0x%03X (ignore)", nnn); // Backward compatiblity
         }
      break;  // 0x0 end

      case 0x1: // 1nnn jump to address
         printf("JP 0x%03X (Jump to add 0x%3X)",nnn,nnn);
         chip8->pc = nnn;
      break; // 0x1 end
      
      case 0x2: // 2nnn - Call subroutine at nnn
         printf("CALL 0x%03X (Call Subroutine at 0x%03X)", nnn,nnn);
         chip8->stack[chip8->sp] = chip8->pc; //save return address
         chip8->sp++;                         // move stack pointer up
         chip8->pc = nnn;                     // jump to subroutines
      break; // 0x2 end

      case 0x3: // 3xkk - Skip if Vx == kk
         printf("SE V%X, 0x%02X (Skip next inst if V[%X]==kk[0x%02X])", x, kk , x , kk);
         if(chip8->V[x] == kk) chip8->pc += 2;
      break; // 0x3 end
      
      case 0x4: // 4xkk - Skip if Vx != kk (opposite of 0x3)
         printf("SNE V%X 0x%03X (Skip next ins if V[%X]!=kk[0x%03X])", x , kk , x , kk);
         if(chip8->V[x] != kk) chip8->pc += 2;
      break; // 0x4 end

      case 0x5: // 5xy0 - Skip if Vx == Vy
         printf("SE V%X, V%X (Skip next inst if V%X==y[V%X])", x, y , x , y);
         if(chip8->V[x] == chip8->V[y]) chip8->pc += 2;
      break; // 0x5 end 

      case 0x6: // 6xkk - LD Vx, byte (Load immediate value into register)
         printf("LD V%X, 0x%02X (Set V%X=kk[0x%2X])", x, kk , x , kk );
         chip8->V[x] = kk;
      break; // 0x6 end 
 
      case 0x7: // 7xkk - Add kk to Vx
         printf("ADD V%X, 0x%02X (Add V%X+=kk[0x%02X])", x, kk , x, kk);
         chip8->V[x] += kk;
      break; // 0x7 end
 
      case 0x8: // 0x8xxx - Arithmetic and Logic Operations
         /* 8xyn
            Where:
            - 8 = opcode identifier
            - x = first register index (0-F)
            - y = second register index (0-F)
            - n = operation type (0-E) */
         switch (n) {
            case 0x0: // 8xy0 - LD Vx, Vy
               printf("LD V%X, V%X (Load Vy into Vx)", x , y );
               chip8->V[x] = chip8->V[y];
            break; // 0x0 end 

            case 0x1: // 8xy1 - OR Vx, Vy
               printf("OR V%X, V%X (BITWISE OR )", x , y );
               chip8->V[x] |= chip8->V[y];
            break; // 0x1 end 

            case 0x2: // 8xy2 - AND Vx, Vy
               printf("AND V%X, V%X (BITWISE AND)", x , y );
               chip8->V[x] &= chip8->V[y];
            break; // 0x2 end 

            case 0x3: // 8xy3 - XOR Vx, Vy
               printf("XOR V%X, V%X (BITWISE XOR)", x , y );
               chip8->V[x] ^= chip8->V[y];
            break; // 0x3 end 

            case 0x4: // 8xy4 - ADD Vx, Vy (with carry)
               printf("ADD V%X, V%X (ADD with CARRY)", x , y );
               uint16_t sum = chip8->V[x] + chip8->V[y];
               chip8->V[x] = sum & 0xFF;
               chip8->V[0xF] = (sum > 0xFF);
            break; // 0x4 end 

            case 0x5: // 8xy5 - SUB Vx, Vy 
               printf("SUB V%X, V%X (SUBTRACT)", x , y );
               chip8->V[0xF] = (chip8->V[x] > chip8->V[y]);
               chip8->V[x] -= chip8->V[y];
            break; // 0x5 end 

            case 0x6: // 8xy6 - SHR Vx, Vy 
               printf("SHR V%X, V%X (SHIFT RIGHT)", x , y );
               chip8->V[0xF] = chip8->V[x] &  0x1; // set carry flag to least significant bit 
               chip8->V[x] >>= 1;                  // shift right by 1
            break; // 0x6 end 

            case 0x7:// 8xy7 - SUB Vx, Vy 
               printf("SUBN V%X, V%X (SUBTRACT REVERSED)", x , y );
               chip8->V[0xF] = (chip8->V[y] > chip8->V[x]);
               chip8->V[x] = chip8->V[y] - chip8->V[x];
            break; // 0x7 end 

            case 0xE:// 8xyE - SHL Vx, Vy
               printf("SHL V%X, V%X (SHIFT LEFT)", x , y );
               chip8->V[0xF] = (chip8->V[x] &  0x80) >> 7; // set carry flag to most significant bit 
               chip8->V[x] <<= 1;                          // shift left by 1
            break; // 0xE end 

         } // switch(n) end 
      break; // 0x8 end

      case 0x9: // 9xy0 - SNE Vx, Vy Skip if Vx != Vy
         printf("SNE V%X, V%X (SKIP IF VX != VY)", x, y);
         if(chip8->V[x] != chip8->V[y]) chip8->pc += 2;
      break; // 0x9
       
      case 0xA: // Annn - LD I, addr (Load Index Register with address)
         printf("LD I, 0x%03X (LOADS 12bit into I)", nnn);
         chip8->I = nnn;
      break; // 0xA end 

      case 0xB: // Bnnn - JP V0, addr (Jump to address V0 + nnn)
        printf("JP V0, 0x%03X (JUMP TO ADDr V0 + nnn)", nnn);
        chip8->pc = nnn + chip8->V[0];
      break; // 0xB end
        
      case 0xC: // Cxkk - RND Vx, byte (Random number generation)
         printf("RND V%X, 0x%02X (RANDOM NO MASK WITH KK)", x, kk);
         chip8->V[x] = (rand() % 256) & kk;
      break; // 0xC end

      case 0xD: // Dxyn - Draw sprite at (Vx, Vy) with n bytes of sprite data
        {
           printf("DRW V%X, V%X, %d (Draw X[%X] Y[%X] HE[%d])", x, y, n , x, y, n);

           uint8_t x_pos = chip8->V[x] % DISPLAY_WIDTH; // X cordinate (wrapped) x_pos = 70 % 64 = 6
           uint8_t y_pos = chip8->V[y] % DISPLAY_HEIGHT;// Y cordinate (wrapped) y_pos = 40 % 32 = 8
           chip8->V[0xF] = 0; // Reset colision flag 
           
           for(int row = 0; row < n; row++) { // process each sprite row 
               //reads the sprite data from memory, one row at a time
              uint8_t sprite_byte = chip8->memory[chip8->I + row];
              
              for(int col = 0; col < 8; col++) {  // process each pixel in row
                  // Bit Extraction: (0x80 >> col) scans bits from left to right (MSB first)
                  /* sprite_byte = 0xA5 (1010 0101 binary)
                     col=0: 0x80 >> 0 = 1000 0000 → checks bit 7 (1) ✓ DRAW
                     col=1: 0x80 >> 1 = 0100 0000 → checks bit 6 (0) ✗ skip  
                     col=2: 0x80 >> 2 = 0010 0000 → checks bit 5 (1) ✓ DRAW */
                 if((sprite_byte & (0x80 >> col)) != 0) { 
                     // calculate the exact screen position for each sprite pixel
                    int pixel_x = (x_pos + col) % DISPLAY_WIDTH;
                    int pixel_y = (y_pos + row) % DISPLAY_HEIGHT;
                     // converts (X,Y) coordinates into a 1D array index
                    int pixel_index = pixel_y * DISPLAY_WIDTH + pixel_x;
                    
                    if(chip8->display[pixel_index] == 1) {
                       chip8->V[0xF] = 1; // Collision detection
                    }
                    chip8->display[pixel_index] ^= 1;
                 }
              }
           }
           chip8->draw_flag = true;
        }
      break; // 0xD ends 

      case 0xE: // Keyboard Input Instructions
        switch(kk) {
           case 0x9E: // Ex9E - Skip if key in Vx is pressed
              printf("SKP V%X (skip Vx if key is press)", x);
              if(chip8->keypad[chip8->V[x]]) chip8->pc += 2;
              break;
           case 0xA1: // ExA1 - Skip if key in Vx is NOT pressed
              printf("SKNP V%X (Skip Vx if key not press)", x);
              if(!chip8->keypad[chip8->V[x]]) chip8->pc += 2;
              break;
        }
      break; // 0xE end
 
      case 0xF: // Miscellaneous Operation handles timers memory sound and input 
         switch (kk) {

            case 0x07: // 0xFx07 - LD Vx, DT (Read Delay Timer)
               printf("LD V%X, DT (Read Delay timer)", x);
               chip8->V[x] = chip8->delay_timer;
            break; // 0x07 end

            case 0x0A: // 0xFx0A - LD Vx, K (Wait for Key Press)
               printf("LD V%X, K (Watit for key press)", x);
               {
                  bool key_pressed = false;
                  for(int i = 0; i < 16; i++) {
                     if(chip8->keypad[i]) {
                        chip8->V[x] = i;
                        key_pressed = true;
                        break;
                     }
                  }
                  if(!key_pressed) chip8->pc -= 2; // Try again next cycle
               }
            break; // 0x0A end

            case 0x15: // 0xFx15 - LD DT, Vx (Set Delay Timer)
               printf("LD DT, V%X (SET DISPLAY TIMER)", x);
               chip8->delay_timer = chip8->V[x];
            break; // 0xF end 

            case 0x18: // 0xFx18 - LD ST, Vx (Set Sound Timer)
               printf("LD ST, V%X (SET SOUND TIMER)", x);
               chip8->sound_timer = chip8->V[x];
            break;

            case 0x1E: // 0xFx1E - ADD I, Vx (Add to Index Register)
               printf("ADD I, V%X (ADD TO INDEX REGISTER)", x);
               chip8->I += chip8->V[x];
            break;

            case 0x29: // 0xFx29 - LD F, Vx (Load Font Character)
               printf("LD F, V%X (LOAD FONT CHARACTER)", x);
               chip8->I = chip8->V[x] * 5; // Each sprite is 5 bytes
            break;

            case 0x33: // 0xFx33 - LD B, Vx (BCD Conversion)
               printf("LD B, V%X (BCD CONVERSION)", x);
               chip8->memory[chip8->I] = chip8->V[x] / 100;
               chip8->memory[chip8->I + 1] = (chip8->V[x] / 10) % 10;
               chip8->memory[chip8->I + 2] = chip8->V[x] % 10;
            break;

            case 0x55: // 0xFx55 - LD [I], Vx (Store Registers to Memory)
               printf("LD [I], V0-V%X (STORE REGISTER INTO MEMORY)", x);
               for(int i = 0; i <= x; i++) {
                  chip8->memory[chip8->I + i] = chip8->V[i];
               }
            break;

            case 0x65: // 0xFx65 - LD Vx, [I] (Load Registers from Memory)
               printf("LD V0-V%X, [I] (LOAD REGISTER INTO MEMORY)", x);
               for(int i = 0; i <= x; i++) {
                  chip8->V[i] = chip8->memory[chip8->I + i];
               }
            break;
 
         } // switch (kk) end
      break; // 0xF end

      default:
         printf("Unknown opcode: 0x%04X\n", opcode);
      break;
 
   } // switch end 

}
