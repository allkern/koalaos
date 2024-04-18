#include "syscall.h"

void syscall(uint32_t num) {
    // Make sure syscall number is in $a0
    asm volatile (
        "move $a0, %0\n"
        "syscall\n"
        : : "r" (num)
    );
}