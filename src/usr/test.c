#include "libc/stdio.h"

#include "sys/dmem.h"
#include "usr/shell.h"

void irq_handler(void) {
    shell_exec("color 2A");
    printf("\nirq_handler: Hello, world!\n");
    shell_exec("color");
}

void syscall(uint32_t num) {
    // Make sure syscall number is in $a0
    asm volatile (
        "move $a0, %0\n"
        : : "r" (num)
    );

    asm("syscall");
}

int usr_test(int argc, const char* argv[]) {
    printf("kernel: Enabling interrupts... ");

    asm volatile (
        ".set noat\n"
        "la      $at, 0x40ff03\n"
        "mtc0    $at, $12\n"
        ".set at\n"
    );

    printf("done\nkernel: Installing IRQ handler at %08x... ", irq_handler);

    syscall(irq_handler);

    puts("done\n");

    asm volatile (
        ".set noat\n"
        "la      $at, 0x40ff03\n"
        "mtc0    $at, $12\n"
        ".set at\n"
    );

    // Crash system
    // *((int*)0) = 0;

    return EXIT_SUCCESS;
}