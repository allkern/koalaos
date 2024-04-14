#ifndef C8_H
#define C8_H

#include "libc/stdint.h"
#include "hw/uart.h"

typedef struct {
    uint8_t* screen;
    uint8_t* mem;
    uint8_t v[16];
    uint16_t i, pc;
    uint8_t dt, st;
    uint16_t stack[16];
    uint8_t sp;
    uint16_t opcode;
    uint16_t rnd;
    uint8_t keypad[16];
} chip8_t;

typedef void (*c8_instruction_t)(chip8_t*);

void c8_load_program(chip8_t* c8, uint8_t* prg, int size);
void c8_keydown(chip8_t* c8, int k);
void c8_keyup(chip8_t* c8, int k);
void c8_i_clr_ret(chip8_t* c8);
void c8_i_jump(chip8_t* c8);
void c8_i_call(chip8_t* c8);
void c8_i_seqi(chip8_t* c8);
void c8_i_snei(chip8_t* c8);
void c8_i_seq(chip8_t* c8);
void c8_i_li(chip8_t* c8);
void c8_i_addi(chip8_t* c8);
void c8_i_move(chip8_t* c8);
void c8_i_or(chip8_t* c8);
void c8_i_and(chip8_t* c8);
void c8_i_xor(chip8_t* c8);
void c8_i_add(chip8_t* c8);
void c8_i_sub(chip8_t* c8);
void c8_i_shr(chip8_t* c8);
void c8_i_subn(chip8_t* c8);
void c8_i_shl(chip8_t* c8);
void c8_i_invalid(chip8_t* c8);
void c8_i_alu(chip8_t* c8);
void c8_i_sne(chip8_t* c8);
void c8_i_ldi(chip8_t* c8);
void c8_i_jr(chip8_t* c8);
void c8_i_rnd(chip8_t* c8);
void c8_i_drw(chip8_t* c8);
void c8_i_key(chip8_t* c8);
void c8_i_mfdt(chip8_t* c8);
void c8_i_wfk(chip8_t* c8);
void c8_i_mtdt(chip8_t* c8);
void c8_i_mtst(chip8_t* c8);
void c8_i_iadd(chip8_t* c8);
void c8_i_chr(chip8_t* c8);
void c8_i_bcd(chip8_t* c8);
void c8_i_stm(chip8_t* c8);
void c8_i_ldm(chip8_t* c8);
void c8_i_io(chip8_t* c8);
void c8_init(chip8_t* c8, uint8_t* screen, uint8_t* mem);
void c8_execute_instruction(chip8_t* c8);

#endif