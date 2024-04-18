#include "libc/string.h"
#include "libc/stdio.h"
#include "sys/ext2.h"

#include "shell.h"

int usr_cat(int argc, const char* argv[]) {
    if (!argv[1]) {
        printf("No input supplied\n");

        return EXIT_FAILURE;
    }

    char path[MAX_PATH];

    shell_get_absolute_path(argv[1], path, MAX_PATH);

    struct ext2_fd file;

    if (ext2_fopen(&file, path, "rb")) {
        printf("Couldn't find path \'%s\'\n", path);

        return EXIT_FAILURE;
    }

    char* buf = malloc(file.inode.s_sizel + 1);

    if (!buf) {
        puts("Could not allocate memory");

        return 1;
    }

    memset(buf, 0, file.inode.s_sizel + 1);

    ext2_fread(&file, buf, file.inode.s_sizel);
    ext2_fclose(&file);

    printf("%s\n", buf);

    free(buf);

    return EXIT_SUCCESS;
}