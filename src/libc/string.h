#ifndef STRING_H
#define STRING_H

void* memset(void *dst, int c, unsigned int n);
int memcmp(const void *v1, const void *v2, unsigned int n);
void* memmove(void *dst, const void *src, unsigned int n);
void* memcpy(void *dst, const void *src, unsigned int n);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *p, const char *q, unsigned int n);
char* strncpy(char *s, const char *t, int n);
char* s_strcpy(char *s, const char *t, int n);
int strlen(const char *s);
char* strchr(char* str, char c);
char* strrchr(char* str, char c);

#endif