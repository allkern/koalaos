#include "libc/stddef.h"
#include "libc/stdio.h"
#include "libc/time.h"

#include "dmem.h"

extern unsigned int __executable_end[];
const uint32_t __ram_end = 0x1000000;
uint8_t __heap_map[0x1000000 >> 12];
uint32_t __heap_start;
uint32_t __heap_table_start;
uint32_t __heap_table_size;

#define MAP_FREE 0x01

void* dmem_alloc(size_t size) {
    int p = (size >> 12) + ((size & 0xfff) ? 1 : 0);
    int k = 0;

    while (1) {
        int i;

        for (i = k; i < __heap_table_size; i++)
            if (__heap_map[i] & MAP_FREE)
                break;

        // Out of memory
        if (i >= __heap_table_size)
            return NULL;

        int j = 1;

        while ((__heap_map[i+j] & MAP_FREE) && (j < p))
            ++j;

        if (j == p) {
            for (int n = 1; n < p; n++)
                __heap_map[i+n] &= ~MAP_FREE;

            __heap_map[i] = p << 1;

            return (unsigned char*)__heap_start + (i << 12);
        } else {
            k += j;
        }
    }
}

int dmem_extend(void* ptr, size_t new_size) {
    uint32_t i = ((uint32_t)ptr - __heap_start) >> 12;

    int s = __heap_map[i] >> 1;
    int p = (new_size >> 12) + ((new_size & 0xfff) ? 1 : 0);

    // Check whether all required blocks past the
    // original size are free, if not, return 0
    for (int j = s; j < p; j++)
        if ((__heap_map[i+j] & MAP_FREE) == 0)
            return 0;

    // If so, then extend
    for (int j = s; j < p; j++)
        __heap_map[i+j] = 0;

    // Update size
    __heap_map[i] = p << 1;

    return 1;
}

void dmem_free(void* ptr) {
    uint32_t i = ((uint32_t)ptr - __heap_start) >> 12;

    int s = __heap_map[i] >> 1;

    for (int j = 0; j < s; j++)
        __heap_map[i+j] = MAP_FREE;
}

size_t dmem_get_alloc_size(void* ptr) {
    uint32_t i = ((uint32_t)ptr - __heap_start) >> 12;

    return (__heap_map[i] >> 1) << 12;
}

struct tlb_entry tlb_read_entry(int index) {
    struct tlb_entry entry;

    asm (
        "mtc0 %2, $0\n"
        "tlbr\n"
        "mfc0 %0, $2\n"
        "mfc0 %1, $10\n"
        : "=r" (entry.lo), "=r" (entry.hi)
        : "r" (index)
    );

    return entry;
}

uint32_t pround(uint32_t ptr) {
    return (ptr & 0xfffff000) + (((ptr & 0xfff) != 0) * 0x1000);
}

void dmem_init(void) {
    uint32_t __exec_end = (uint32_t)__executable_end;

    __heap_start = pround(__exec_end);

    uint32_t __heap_end = pround(__ram_end);
    uint32_t __heap_size = __heap_end - __heap_start;

    int size = 0;
    char* unit = "B";

    if (__heap_size < 1024) {
        unit = "B";
        size = __heap_size;
    } else if (__heap_size >= 1024 && __heap_size < (1024 * 1024)) {
        unit = "KiB";
        size = __heap_size / 1024;
    } else if (__heap_size >= (1024 * 1024) && __heap_size < (1024 * 1024 * 1024)) {
        unit = "MiB";
        size = __heap_size / (1024 * 1024);
    } else {
        unit = "GiB";
        size = __heap_size / (1024 * 1024 * 1024);
    }

    // printf("Heap space available: %d %s (%d bytes)\n", size, unit, __heap_size);

    __heap_table_size = __heap_size >> 12;

    // printf("dmem_alloc=%08x\n", dmem_alloc(0x2000));

    for (int i = 0; i < __heap_table_size; i++)
        __heap_map[i] = 1;
}