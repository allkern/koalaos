// license:MIT
// copyright-holders:Lisandro Alarcon (Allkern)

/**
 * @file r3000d.h
 * @brief Disassembler for MIPS R3000A compatible code
 * @author Allkern (https://github.com/allkern)
 */

#ifndef R3000D_H
#define R3000D_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libc/stdint.h"

typedef struct {
    uint32_t addr;
    int print_address;
    int print_opcode;
    int hex_memory_offset;
} r3000d_state;

/** @brief Disassemble a single opcode, printing to a buffer and
 *         optionally taking in a pointer to a disassembler state
 *         struct
 *
 *  @param buf pointer to a char buffer
 *  @param opcode opcode to disassemble
 *  @param state optional pointer to disassembler state struct
 *         (pass NULL if not required)
 *  @returns `buf`
 */
char* r3000d_disassemble(char* buf, uint32_t opcode, r3000d_state* state);

#ifdef __cplusplus
}
#endif

#endif