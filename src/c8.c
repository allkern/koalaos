#include "libc/stdint.h"
#include "hw/uart.h"

#include "c8.h"

void c8_load_program(chip8_t* c8, uint8_t* prg, int size) {
    for (int i = 0; i < size; i++)
        c8->mem[0x200 + i] = prg[i];
}

void c8_keydown(chip8_t* c8, int k) {
    c8->keypad[k] = 1;
}

void c8_keyup(chip8_t* c8, int k) {
    c8->keypad[k] = 0;
}

#define ADDR (c8->opcode & 0xfff)
#define X ((c8->opcode >> 8) & 0xf)
#define Y ((c8->opcode >> 4) & 0xf)
#define I8 (c8->opcode & 0xff)
#define I4 (c8->opcode & 0xf)

void c8_i_clr_ret(chip8_t* c8) {
    // Clear screen
    if (I8 == 0xe0) {
        for (int i = 0; i < (64 * 32); i++)
            c8->screen[i] = 0;
    } else if (I8 == 0xee) {
        c8->pc = c8->stack[--c8->sp];
    } else {
        // Unknown instruction
    }
}

void c8_i_jump(chip8_t* c8) {
    c8->pc = ADDR;
}

void c8_i_call(chip8_t* c8) {
    c8->stack[c8->sp++] = c8->pc;

    c8->pc = ADDR;
}

void c8_i_seqi(chip8_t* c8) {
    if (c8->v[X] == I8)
        c8->pc += 2;
}

void c8_i_snei(chip8_t* c8) {
    if (c8->v[X] != I8)
        c8->pc += 2;
}

void c8_i_seq(chip8_t* c8) {
    if (c8->v[X] == c8->v[Y])
        c8->pc += 2;
}

void c8_i_li(chip8_t* c8) {
    c8->v[X] = I8;
}

void c8_i_addi(chip8_t* c8) {
    c8->v[X] += I8;
}

void c8_i_move(chip8_t* c8) {
    c8->v[X] = c8->v[Y];
}

void c8_i_or(chip8_t* c8) {
    c8->v[X] |= c8->v[Y];
}

void c8_i_and(chip8_t* c8) {
    c8->v[X] &= c8->v[Y];
}

void c8_i_xor(chip8_t* c8) {
    c8->v[X] ^= c8->v[Y];
}

void c8_i_add(chip8_t* c8) {
    uint16_t r = (uint16_t)c8->v[X] + (uint16_t)c8->v[Y];

    c8->v[X] = r & 0xff;
    c8->v[15] = r > 0xff;
}

void c8_i_sub(chip8_t* c8) {
    int flag = c8->v[X] >= c8->v[Y];

    c8->v[X] -= c8->v[Y];
    c8->v[15] = flag;
}

void c8_i_shr(chip8_t* c8) {
    int lsb = c8->v[Y] & 1;

    c8->v[X] >>= 1;
    c8->v[15] = lsb != 0;
}

void c8_i_subn(chip8_t* c8) {
    int flag = c8->v[Y] >= c8->v[X];

    c8->v[X] = c8->v[Y] - c8->v[X];
    c8->v[15] = flag;
}

void c8_i_shl(chip8_t* c8) {
    int msb = c8->v[X] & 0x80;

    c8->v[X] <<= 1;
    c8->v[15] = msb != 0;
}

void c8_i_invalid(chip8_t* c8) {
    return;
}

static c8_instruction_t g_c8_8_instruction_table[] = {
    c8_i_move,
    c8_i_or,
    c8_i_and,
    c8_i_xor,
    c8_i_add,
    c8_i_sub,
    c8_i_shr,
    c8_i_subn,
    c8_i_invalid,
    c8_i_invalid,
    c8_i_invalid,
    c8_i_invalid,
    c8_i_invalid,
    c8_i_invalid,
    c8_i_shl,
    c8_i_invalid
};

void c8_i_alu(chip8_t* c8) {
    g_c8_8_instruction_table[c8->opcode & 0xf](c8);
}

void c8_i_sne(chip8_t* c8) {
    if (c8->v[X] != c8->v[Y])
        c8->pc += 2;
}

void c8_i_ldi(chip8_t* c8) {
    c8->i = ADDR;
}

void c8_i_jr(chip8_t* c8) {
    c8->pc = ADDR + (uint16_t)c8->v[0];
}

void c8_i_rnd(chip8_t* c8) {
    c8->v[X] = c8->rnd & I8;
}

#define VX c8->v[X]
#define VY c8->v[Y]
#define SCREEN(x, y) c8->screen[(x) + ((y) * 64)]

void c8_i_drw(chip8_t* c8) {
    for (int y = 0; y < I4; y++) {
        uint8_t data = c8->mem[c8->i + y];

        for (int x = 0; x < 8; x++) {
            int b = (data & (0x80 >> x)) != 0;

            c8->v[15] |= SCREEN(VX + x, VY + y) && b;

            SCREEN(VX + x, VY + y) ^= b;
        }
    }
}

void c8_i_key(chip8_t* c8) {
    if (I8 == 0x9e) {
        if (c8->keypad[c8->v[X] & 0xf])
            c8->pc += 2;
    } else if (I8 == 0xa1) {
        if (!c8->keypad[c8->v[X] & 0xf])
            c8->pc += 2;
    } else {
        // Invalid instruction
    }
}

void c8_i_mfdt(chip8_t* c8) {
    c8->v[X] = c8->dt;
}

void c8_i_wfk(chip8_t* c8) {
    int k = 0;

    for (int i = 0; i < 16; i++)
        k |= c8->keypad[i];

    if (!k)
        c8->pc -= 2;
}

void c8_i_mtdt(chip8_t* c8) {
    c8->dt = c8->v[X];
}

void c8_i_mtst(chip8_t* c8) {
    c8->st = c8->v[X];
}

void c8_i_iadd(chip8_t* c8) {
    c8->i += (uint16_t)c8->v[X];
}

void c8_i_chr(chip8_t* c8) {
    c8->i = c8->v[X] * 5;
}

void c8_i_bcd(chip8_t* c8) {
    int vx = c8->v[X];

    c8->mem[c8->i + 0] = (vx / 100) % 10;
    c8->mem[c8->i + 1] = (vx / 10) % 10;
    c8->mem[c8->i + 2] = vx % 10;
}

void c8_i_stm(chip8_t* c8) {
    for (int i = 0; i <= X; i++)
        c8->mem[c8->i++] = c8->v[i];
}

void c8_i_ldm(chip8_t* c8) {
    for (int i = 0; i <= X; i++)
        c8->v[i] = c8->mem[c8->i++];
}

void c8_i_io(chip8_t* c8) {
    switch (I8) {
        case 0x07: c8_i_mfdt(c8); break;
        case 0x0a: c8_i_wfk(c8); break;
        case 0x15: c8_i_mtdt(c8); break;
        case 0x18: c8_i_mtst(c8); break;
        case 0x1e: c8_i_iadd(c8); break;
        case 0x29: c8_i_chr(c8); break;
        case 0x33: c8_i_bcd(c8); break;
        case 0x55: c8_i_stm(c8); break;
        case 0x65: c8_i_ldm(c8); break;
    }
}

static c8_instruction_t g_c8_instruction_table[] = {
    c8_i_clr_ret, c8_i_jump   , c8_i_call   , c8_i_seqi   ,
    c8_i_snei   , c8_i_seq    , c8_i_li     , c8_i_addi   ,
    c8_i_alu    , c8_i_sne    , c8_i_ldi    , c8_i_jr     ,
    c8_i_rnd    , c8_i_drw    , c8_i_key    , c8_i_io
};

static uint8_t g_c8_font[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,
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
    0xF0, 0x80, 0xF0, 0x80, 0xF0,
    0xF0, 0x80, 0xF0, 0x80, 0x80
};

void c8_init(chip8_t* c8, uint8_t* screen, uint8_t* mem) {
    c8->screen = screen;
    c8->mem = mem;
    c8->pc = 0x200;
    c8->dt = 0;
    c8->st = 0;
    c8->i = 0;
    c8->sp = 0;

    for (int i = 0; i < (64 * 32); i++)
            c8->screen[i] = 0;
    
    for (int i = 0; i < 16; i++)
        c8->stack[i] = 0;

    for (int i = 0; i < 16; i++)
        c8->v[i] = 0;
    
    for (int i = 0; i < 80; i++)
        c8->mem[i] = g_c8_font[i];
}

void c8_execute_instruction(chip8_t* c8) {
    c8->opcode = (((uint16_t)c8->mem[c8->pc]) << 8) | c8->mem[c8->pc + 1];
    c8->pc += 2;

    g_c8_instruction_table[c8->opcode >> 12](c8);

    // Random LFSR
    c8->rnd >>= 1;
    
    if ((c8->rnd ^ (c8->rnd >> 1) ^ 1) & 1) {
        c8->rnd |= 0x4040;
    } else {
        c8->rnd &= ~0x4040;
    }

    if (c8->dt)
        --c8->dt;

    if (c8->st)
        --c8->st;
}