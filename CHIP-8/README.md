# CHIP-8 Emulator

A CHIP-8 emulator using SDL2 and C.

### Prerequisites

- GCC compiler
- SDL2 development libraries

### build
```bash
make
```

## Usage

```bash
./build/chip8 <path-to-rom>
```

### Key Mapping
The CHIP-8 hexadecimal keypad (4×4) is mapped in keyboard:

```
 Keyboard Layout
┌───┬───┬───┬───┐
│ 1 │ 2 │ 3 │ 4 │
├───┼───┼───┼───┤
│ Q │ W │ E │ R │
├───┼───┼───┼───┤
│ A │ S │ D │ F │
├───┼───┼───┼───┤
│ Z │ X │ C │ V │
└───┴───┴───┴───┘
```

- **ESC**: Exit the emulator

---
