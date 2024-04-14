#ifndef USER_H
#define USER_H

#include "ext2.h"

#define USER_NAME_MAXLEN 30
#define USER_PASS_MAXLEN 30

struct __attribute__((packed)) sys_user {
    char name[USER_NAME_MAXLEN];
    char pass[USER_PASS_MAXLEN];
    uint16_t id;
    uint16_t flags;
};

#define MAX_USERS (EXT2_SECTOR_SIZE / sizeof(struct sys_user))

enum {
    LOGIN_OK,
    LOGIN_WRONG_PASS,
    LOGIN_NO_USER
};

int user_init(void);
// void user_create(void);
int user_login(const char* name, const char* pass);
struct sys_user* user_lookup_by_name(const char* name);
struct sys_user* user_lookup_by_id(uint32_t id);

char* user_get_name(void);
uint16_t user_get_id(void);
int user_is_root(void);
void user_get_home_path(char* buf);

#endif