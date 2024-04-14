#include "libc/string.h"
#include "libc/stdio.h"

#include "user.h"
#include "ext2.h"

struct sys_user users[EXT2_SECTOR_SIZE / sizeof(struct sys_user)];

static struct sys_user* curr;

int user_init(void) {
    struct ext2_fd fd;

    if (ext2_fopen(&fd, "/etc/users", "rb")) {
        printf("Could not open users file, can't initialize user system\n");

        return 1;
    }

    // Cache users file
    ext2_fread(&fd, users, EXT2_SECTOR_SIZE);
    ext2_fclose(&fd);

    return 0;
}

// void user_create(void);
int user_login(const char* name, const char* pass) {
    struct sys_user* user = user_lookup_by_name(name);

    if (!user)
        return LOGIN_NO_USER;

    int len = strlen(pass);

    if (strlen(user->pass) != len)
        return LOGIN_WRONG_PASS;

    if (strncmp(users->pass, pass, len))
        return LOGIN_WRONG_PASS;

    curr = user;

    return LOGIN_OK;
}

struct sys_user* user_lookup_by_name(const char* name) {
    int len = strlen(name);

    for (int i = 0; i < MAX_USERS; i++) {
        if (strlen(users[i].name) != len)
            continue;

        if (!strncmp(users[i].name, name, len))
            return &users[i];
    }

    return NULL;
}

struct sys_user* user_lookup_by_id(uint32_t id) {
    for (int i = 0; i < MAX_USERS; i += sizeof(struct sys_user))
        if (users[i].id == id)
            return &users[i];

    return NULL;
}

char* user_get_name(void) {
    return curr->name;
}

uint16_t user_get_id(void) {
    return curr->id;
}

int user_is_root(void) {
    return curr->flags & 1;
}

void user_get_home_path(char* buf) {
    sprintf(buf, "/usr/%s/", curr->name);
}