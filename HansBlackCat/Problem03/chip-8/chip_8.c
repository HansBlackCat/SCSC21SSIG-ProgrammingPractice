#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "chip_8.h"
#include "dbg.h"

#define NNN(opcode) (uint16_t)((opcode)&0x0FFF)
#define NN(opcode) (uint16_t)((opcode)&0x00FF)
#define N(opcode) (uint16_t)((opcode)&0x000F)
#define X(opcode) (uint16_t)((opcode)&0x0F00)
#define Y(opcode) (uint16_t)((opcode)&0x00F0)
#define RND() (rand() % 256)

byte font_sprite[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70,
    0xF0, 0x10, 0xF0, 0x80, 0xF0,
    0xF0, 0x10, 0xF0, 0x10, 0xF0,
    0x90, 0x90, 0xF0, 0x10, 0x10,
    0xF0, 0x80, 0xF0, 0x10, 0xF0,
    0xF0, 0x80, 0xF0, 0x90, 0xF0,
    0xF0, 0x10, 0x20, 0x40, 0x40,
    0xF0, 0x90, 0xF0, 0x90, 0xF0,
    0xF0, 0x90, 0xF0, 0x10, 0xF0,
    0xF0, 0x90, 0xF0, 0x90, 0x90,
    0xE0, 0x90, 0xE0, 0x90, 0xE0,
    0xF0, 0x80, 0x80, 0x80, 0xF0,
    0xE0, 0x90, 0x90, 0x90, 0xE0,
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // F
};

uint8_t NN;
uint8_t Reg[REG_NUMS];
uint8_t Memory[TOTAL_MEMEORY_SIZE];
uint16_t delay_timer;
uint16_t sound_timer;
uint16_t NNN; // Address
uint16_t I;   // Memory address Reg
uint16_t PC;  // Program counter
uint16_t opcode;
uint16_t SP;
uint16_t gfx[DISPLAY_RESOLUTION_TOT_SIZE];
uint16_t stack[STACK_SIZE];

static inline instruction_branching(uint16_t opcode);

static inline instruction_branching(uint16_t opcode)
{

  switch (opcode & 0xF000)
  {
  case 0x0000:
    switch (opcode & 0x0FFF)
    {
    case 0x00E0: // Display - clean
      debug("0x%x: Clear screen", opcode);
      memset(gfx, 0, sizeof(gfx[0]) * DISPLAY_RESOLUTION_TOT_SIZE);
      PC += 2;
      break;
    case 0x00EE: // Flow - return
      debug("0x%x: Returns from subroutine", opcode);
      // TODO
      break;
    default: // Call
      // Not necessary
      break;
    }
  case 0x1000: // Flow - goto
    debug("0x%x: Jumps to addr %x", opcode, NNN(opcode));
    PC = NNN(opcode);
    break;
  case 0x2000: // Flow - suobroutine
    debug("0x%x: Calls subroutine at %x", opcode, NNN(opcode));
    PC += 2;
    stack[SP++] = PC;
    PC = NNN(opcode);
    break;
  case 0x3000: // Cond - Vx == NN
    debug("0x%x: Skips next inst if V%x == 0x%x", opcode, Reg[X(opcode)], NN(opcode));
    PC += (Reg[X(opcode)] == NN(opcode)) ? 4 : 2;
    break;
  case 0x4000: // Cond - Vx != NN
    debug("0x%x: Skips next inst if V%x != 0x%x", opcode, Reg[X(opcode)], NN(opcode));
    PC += (Reg[X(opcode)] != NN(opcode)) ? 4 : 2;
    break;
  case 0x5000: // Cond - Vx == Vy
    debug("0x%x: Skips next inst if V%x == V%x", opcode, Reg[X(opcode)], Reg[Y(opcode)]);
    PC += (Reg[X(opcode)] == Reg[Y(opcode)]) ? 4 : 2;
    break;
  case 0x6000: // Const - Vx = N
    debug("0x%x: Sets V%x to 0x%x", opcode, Reg[X(opcode)], NN(opcode));
    Reg[X(opcode)] = NN(opcode);
    PC += 2;
    break;
  case 0x7000: // Const - Vx += N
    debug("0x%x: Adds 0x%x to V%x", opcode, NN(opcode), Reg[X(opcode)]);
    Reg[X(opcode)] += NN(opcode);
    PC += 2;
    break;
  case 0x8000:
    switch (opcode & 0x000F)
    {
    case 0x0000: // Assig Vx = Vy
      debug("0x%x: Sets V%x to the value of V%x", opcode, Reg[X(opcode)], Reg[Y(opcode)]);
      Reg[X(opcode)] = Reg[Y(opcode)];
      PC += 2;
      break;
    case 0x0001: // BitOp Vx = Vx | Vy
      debug("0x%x: Sets V%x = V%x | V%x", opcode, Reg[X(opcode)], Reg[X(opcode)], Reg[Y(opcode)]);
      Reg[X(opcode)] = Reg[X(opcode)] | Reg[Y(opcode)];
      PC += 2;
      break;
    case 0x0002: // BitOp Vx = Vx & Vy
      debug("0x%x: Sets V%x = V%x & V%x", opcode, Reg[X(opcode)], Reg[X(opcode)], Reg[Y(opcode)]);
      Reg[X(opcode)] = Reg[X(opcode)] & Reg[Y(opcode)];
      PC += 2;
      break;
    case 0x0003: // BitOp Vx = Vx ^ Vy
      debug("0x%x: Sets V%x = V%x ^ V%x", opcode, Reg[X(opcode)], Reg[X(opcode)], Reg[Y(opcode)]);
      Reg[X(opcode)] = Reg[X(opcode)] ^ Reg[Y(opcode)];
      PC += 2;
      break;
    case 0x0004: // Math Vx += Vy
      debug("0x%x: V%x += V%x", opcode, Reg[X(opcode)], Reg[Y(opcode)]);
      Reg[0xF] = (((int)Reg[X(opcode)] + (int)Reg[Y(opcode)]) > 255) ? 1 : 0;
      Reg[X(opcode)] += Reg[Y(opcode)];
      PC += 2;
      break;
    case 0x0005: // Math Vx -= Vy
      debug("0x%x: V%x -= V%x", opcode, Reg[X(opcode)], Reg[Y(opcode)]);
      Reg[0xF] = (Reg[X(opcode)] >= Reg[Y(opcode)]) ? 1 : 0;
      Reg[X(opcode)] -= Reg[Y(opcode)];
      PC += 2;
      break;
    case 0x0006: // BitOp Vx >>= 1
      debug("0x%x: V%x >>= 1", opcode, Reg[X(opcode)]);
      Reg[0xF] = (Reg[X(opcode)] & 0x0001);
      Reg[X(opcode)] >>= 1;
      PC += 2;
      break;
    case 0x0007: // Math Vx = Vy - Vx
      debug("0x%x: V%x = V%x - V%x", opcode, Reg[X(opcode)], Reg[Y(opcode)], Reg[X(opcode)]);
      Reg[0xF] = (Reg[Y(opcode)] > Reg[X(opcode)]) ? 1 : 0;
      Reg[X(opcode)] = Reg[Y(opcode)] - Reg[X(opcode)];
      PC += 2;
      break;
    case 0x000E: // BitOp Vx <<= 1
      debug("0x%x: V%x <<= 1", opcode, Reg[X(opcode)]);
      Reg[0xF] = (Reg[X(opcode)] & 0x1000);
      Reg[X(opcode)] <<= 1;
      PC += 2;
      break;
    }
  case 0x9000: // Cond Vx != Vy
    debug("0x%x: Skips next inst if V%x != V%x", opcode, Reg[X(opcode)], Reg[Y(opcode)]);
    PC += (Reg[X(opcode)] != Reg[Y(opcode)]) ? 4 : 2;
    break;
  case 0xA000:
    debug("0x%x: Sets I to addr 0x%x", opcode, NNN(opcode));
    I = NNN(opcode);
    PC += 2;
    break;
  case 0xB000:
    debug("0x%x: Jumps to addr 0x%x + V0", opcode, NNN(opcode));
    Reg[0xF] = ((int)(Reg[0x0]) + (int)(NNN(opcode)) > 255) ? 1 : 0;
    PC += Reg[0x0] + NNN(opcode);
    break;
  case 0xC000:
    debug("0x%x: Sets Vx = rand() & 0x%x", opcode, NN(opcode));
    Reg[X(opcode)] = RND() & NN(opcode);
    PC += 2;
    break;
  case 0xD000:
    debug("0x%x: draw sprite at coord (V%x, V%x) - 8 x (%x+1)", opcode, Reg[X(opcode)], Reg[Y(opcode)], N(opcode));
    // TODO
    PC += 2;
    break;
  case 0xE000: // Keyboard
    debug("0x%x: ", opcode);
    PC += 2;
    break;
  case 0xF000:
    switch (opcode & 0x00FF)
    {
    case 0x0007: // Timer
      debug("0x%x: Sets V%x = delay(0x%x)", opcode, Reg[X[opcode]], delay);
      Reg[X(opcode)] = delay_timer;
      PC += 2;
      break;
    case 0x000A: // Keyboard
      debug("0x%x: ", opcode);
      PC += 2;
      break;
    case 0x0015:
      debug("0x%x: Sets delay(0x%x) = V%x", opcode, delay_timer, Reg[X(opcode)]);
      delay_timer = Reg[X(opcode)];
      PC += 2;
      break;
    case 0x0018:
      debug("0x%x: Sets sound(0x%x) = V%x", opcode, sound_timer, Reg[X(opcode)]);
      sound_timer = Reg[X(opcode)];
      PC += 2;
      break;
    case 0x001E:
      debug("0x%x: I(0x%x) += V%x", opcode, I, Reg[X(opcode)]);
      I += Reg[X(opcode)];
      PC += 2;
      break;
    case 0x0029:
      debug("0x%x: I(0x%x) = sprite_addr[V%x]", opcode, I, Reg[X(opcode)]);
      // TODO
      PC += 2;
      break;
    case 0x0033:
      debug("0x%x: Stores BCD representation of V%x", opcode, Reg[X(opcode)]);
      Memory[I] = (int)Reg[X(opcode)] % 1000 / 100;
      Memory[I + 1] = (int)Reg[X(opcode)] % 100 / 10;
      Memory[I + 2] = (int)Reg[X(opcode)] % 10;
      PC += 2;
      break;
    case 0x0055:
      debug("0x%x: Stores V0 to VX", opcode);
      for (int i = 0; i <= X(opcode); ++i)
        Memory[I + i] = Reg[i];
      I += X(opcode) + 1;
      PC += 2;
      break;
    case 0x0065:
      debug("0x%x: Files V0 to VX", opcode);
      for (int i = 0; i <= X(opcode); ++i)
        Reg[i] = Memory[I + i];
      PC += 2;
      break;
    }
  }
}