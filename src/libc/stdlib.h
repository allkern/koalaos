#ifndef STDLIB_H
#define STDLIB_H

#include "stddef.h"

#define	EXIT_FAILURE 1
#define	EXIT_SUCCESS 0

void __libc_init_stdlib();

void* malloc(size_t size);
void* calloc(size_t num, size_t size);
void* realloc(void* ptr, size_t new_size);
void free(void* ptr);
void exit();
int atexit(void (*func)(void));
int atoi(const char *s);
unsigned long strtoul(const char* nptr, char** endptr, int base);

#endif