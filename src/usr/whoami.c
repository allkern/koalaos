#include "libc/stdio.h"

#include "sys/user.h"

int usr_whoami(int argc, const char* argv[]) {
    printf("%s\n", user_get_name());

    return EXIT_SUCCESS;
}