#include "libc/string.h"
#include "libc/stdio.h"
#include "libc/ctype.h"

#include "r3000d.h"

int usr_dis(int argc, const char* argv[]) {
    if (!argv[1])
        return 1;

    char* end = NULL;

    uint32_t addr = strtoul(argv[1], &end, 16);
    int len = 16;

    if (argv[2])
        len = strtoul(argv[2], &end, 0);

    if (!len)
        len = 16;

    uint32_t* ptr = (uint32_t*)addr;

    r3000d_state state;

    state.hex_memory_offset = 0;
    state.print_address = 1;
    state.print_opcode = 1;

    while (len) {
        char buf[128];

        state.addr = (uintptr_t)ptr;

        printf("%s\n", r3000d_disassemble(buf, *ptr++, &state));

        --len;
    }

    return EXIT_SUCCESS;
}