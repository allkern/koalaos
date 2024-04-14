#include "libc/string.h"
#include "libc/stdio.h"
#include "sys/ext2.h"

#include "shell.h"

int usr_cat(int argc, const char* argv[]) {
    char buf[EXT2_SECTOR_SIZE];

    struct ext2_inode inode;

    if (!argv[1]) {
        printf("No input supplied\n");

        return EXIT_FAILURE;
    }

    char path[256];

    shell_get_absolute_path(argv[1], path, 256);

    if (ext2_search(&inode, path)) {
        printf("Couldn't find path \'%s\'\n", path);

        return EXIT_FAILURE;
    }

    if ((inode.s_tp & 0xf000) != INODE_FILE) {
        printf("Path \'%s\' is not a file\n", path);

        return EXIT_FAILURE;
    }

    struct ext2_fd file;

    ext2_fopen(&file, path, "rb");
    ext2_fread(&file, buf, file.inode.s_sizel);
    ext2_fclose(&file);

    printf("%s\n", buf);

    return EXIT_SUCCESS;
}