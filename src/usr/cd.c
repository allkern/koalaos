#include "libc/string.h"
#include "libc/stdio.h"
#include "sys/ext2.h"

#include "shell.h"

int usr_cd(int argc, const char* argv[]) {
    struct ext2_inode inode;

    char path[MAX_PATH];

    shell_get_absolute_path(argv[1], path, MAX_PATH);

    int len = strlen(path);

    if (path[len-1] != '/') {
        path[len] = '/';
        path[len+1] = '\0';
    }

    if (ext2_search(&inode, path)) {
        printf("Couldn't find path \'%s\'\n", path);

        return EXIT_FAILURE;
    }

    if ((inode.s_tp & 0xf000) != INODE_DIRECTORY) {
        printf("Path \'%s\' is not a directory\n");

        return EXIT_FAILURE;
    }

    shell_set_cwd(path);

    return EXIT_SUCCESS;
}