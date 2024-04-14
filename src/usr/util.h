#ifndef UTIL_H
#define UTIL_H

#define SEF_REGISTER(cmd) \
    shell_register(usr_ ## cmd, USR_NAME_ ## cmd, USR_DESC_ ## cmd, 0)

#define SEF_ALIAS(cmd, al) \
    shell_register(usr_ ## cmd, al, "", 1)

#define SEF_DEFINE(cmd, desc) \
    int usr_ ## cmd(int argc, const char* argv[]); \
    static const char* USR_NAME_ ##cmd = #cmd; \
    static const char* USR_DESC_ ##cmd = desc;

#endif