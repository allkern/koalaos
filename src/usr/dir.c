#include "sys/ext2.h"
#include "usr/shell.h"

#include "libc/string.h"
#include "libc/stdlib.h"
#include "libc/stdio.h"
#include "libc/time.h"

void dir_print(struct ext2_dirent* entry) {
    if (entry->s_name[0] == '.')
        return;

    struct ext2_inode inode;

    ext2_get_inode(&inode, entry->s_inode);

    time_t t = inode.s_creation_time;

    struct tm* tm = gmtime(&t);

    char buf[128];

    strftime(buf, 128, "%b %e %H:%M", tm);

    const char* user = "user";

    if (inode.s_user_id == 0)
        user = "root";

    printf("%c%c%c%c%c%c%c%c%c%c %s %9u %s ",
        (entry->s_type == DIRENT_DIRECTORY) ? 'd' : '-',
        (inode.s_tp & PERM_USER_R) ? 'r' : '-',
        (inode.s_tp & PERM_USER_W) ? 'w' : '-',
        (inode.s_tp & PERM_USER_X) ? 'x' : '-',
        (inode.s_tp & PERM_GROUP_R) ? 'r' : '-',
        (inode.s_tp & PERM_GROUP_W) ? 'w' : '-',
        (inode.s_tp & PERM_GROUP_X) ? 'x' : '-',
        (inode.s_tp & PERM_OTHER_R) ? 'r' : '-',
        (inode.s_tp & PERM_OTHER_W) ? 'w' : '-',
        (inode.s_tp & PERM_OTHER_X) ? 'x' : '-',
        user,
        inode.s_sizel, buf
    );

    for (int i = 0; i < entry->s_name_len; i++)
        putchar(entry->s_name[i]);

    if (entry->s_type == DIRENT_DIRECTORY)
        putchar('/');

    putchar('\n');
}

int usr_dir(int argc, const char* argv[]) {
    struct ext2_inode inode;

    char path[256];

    if (!argv[1])
        argv[1] = shell_get_cwd();

    shell_get_absolute_path(argv[1], path, 256);

    if (ext2_search(&inode, path)) {
        printf("Couldn't find path \'%s\'\n", path);

        return EXIT_FAILURE;
    }

    if ((inode.s_tp & 0xf000) != INODE_DIRECTORY) {
        time_t t = inode.s_creation_time;

        struct tm* tm = gmtime(&t);

        char buf[128];

        strftime(buf, 128, "%b %e %H:%M", tm);

        const char* user = "user";

        if (inode.s_user_id == 0)
            user = "root";

        printf("%c%c%c%c%c%c%c%c%c%c %s %9u %s %s\n",
            ((inode.s_tp & 0xf000) == DIRENT_DIRECTORY) ? 'd' : '-',
            (inode.s_tp & PERM_USER_R) ? 'r' : '-',
            (inode.s_tp & PERM_USER_W) ? 'w' : '-',
            (inode.s_tp & PERM_USER_X) ? 'x' : '-',
            (inode.s_tp & PERM_GROUP_R) ? 'r' : '-',
            (inode.s_tp & PERM_GROUP_W) ? 'w' : '-',
            (inode.s_tp & PERM_GROUP_X) ? 'x' : '-',
            (inode.s_tp & PERM_OTHER_R) ? 'r' : '-',
            (inode.s_tp & PERM_OTHER_W) ? 'w' : '-',
            (inode.s_tp & PERM_OTHER_X) ? 'x' : '-',
            user,
            inode.s_sizel, buf,
            path
        );

        return EXIT_SUCCESS;
    }

    ext2_dir_iterate(&inode, dir_print);

    putchar('\n');

    return EXIT_SUCCESS;
}