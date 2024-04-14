// gen.c: Generates a "users" file for KoalaOS

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define NAME_MAXLEN 30
#define PASS_MAXLEN 30
#define SECTOR_SIZE 1024

struct __attribute__((packed)) sys_user {
    char name[NAME_MAXLEN];
    char pass[PASS_MAXLEN];
    uint16_t id;
    uint16_t flags;
} users[SECTOR_SIZE / sizeof(struct sys_user)];

int main(void) {
    FILE* file = fopen("root/etc/users", "wb");

    memset(users, 0, SECTOR_SIZE);

    users[0].id = 0x0000;
    users[0].flags = 0x0001;
    
    strncpy(users[0].name, "root", NAME_MAXLEN);
    strncpy(users[0].pass, "root", PASS_MAXLEN);

    users[1].id = 0x0000;
    users[1].flags = 0x0000;
    
    strncpy(users[1].name, "user", NAME_MAXLEN);
    strncpy(users[1].pass, "user", PASS_MAXLEN);

    fwrite((void*)users, 1, SECTOR_SIZE, file);

    return 0;
}
