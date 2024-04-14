#ifndef DMEM_H
#define DMEM_H

#include "libc/stdint.h"
#include "libc/stddef.h"

#define TLB_ENTRY_COUNT 64

struct tlb_entry {
    uint32_t lo;
    uint32_t hi;
};

struct tlb_entry tlb_read_entry(int index);
void dmem_init(void);
void* dmem_alloc(size_t size);
int dmem_extend(void* ptr, size_t new_size);
void dmem_free(void* ptr);
size_t dmem_get_alloc_size(void* ptr);

#endif