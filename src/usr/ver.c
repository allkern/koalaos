#include "libc/string.h"
#include "libc/stdio.h"

#include "config.h"

int usr_ver(int argc, const char* argv[]) {
    puts(
        "KoalaOS 0.1-"
        STR(COMMIT_HASH)
        " (" __COMPILER__ " " __ARCH__ " " __VERSION__ " " STR(OS_INFO) ")"
    );

    puts("\nCopyright (C) 2024 Allkern/Lisandro Alarcon\n");

    return EXIT_SUCCESS;
}