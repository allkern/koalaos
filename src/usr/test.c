#include "libc/stdio.h"

#include "sys/syscall.h"
#include "sys/dmem.h"
#include "usr/shell.h"

void irq_handler(void) {
    shell_exec("color 2A");
    printf("\nirq_handler: Hello, world!\n");
    shell_exec("color");
}

int usr_test(int argc, const char* argv[]) {
    syscall(0);

    return EXIT_SUCCESS;
}