#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "ctype.h"

void* __ram_start = 0;

uint8_t __memory[0x1000 * 32];
__alloc_block __blocks[32];

static __alloc_block* __alloc_blocks;
static int __alloc_blocks_size;

#define ATEXIT_MAX 32

void (*__atexit_func_array[ATEXIT_MAX])(void);
static int __atexit_func_index;

void __libc_init_stdlib(void) {
    __ram_start = __memory;
    __alloc_blocks = __blocks;
    __alloc_blocks_size = 32;

    for (int i = 0; i < 32; i++)
        __blocks[i].free = 1;
}

int atexit(void (*func)(void)) {
    __atexit_func_array[__atexit_func_index++] = func;
}

void exit(int code) {
    for (int i = 0; i < __atexit_func_index && i < ATEXIT_MAX; i++)
        __atexit_func_array[i]();
}

/*
    Very bad but simple dynamic memory implementation.
    Fixed block size (4 KiB)
*/
void* malloc(size_t size) {
    int i;

    for (i = 0; i < __alloc_blocks_size; i++)
        if (__alloc_blocks[i].free)
            break;

    __alloc_blocks[i].free = 0;

    return __ram_start + (i << ALLOC_BLOCK_SHIFT);
}

void* calloc(size_t num, size_t size) {
    size_t s = num * size;

    void* ptr = malloc(s);

    for (int i = 0; i < s; i++)
        *(unsigned char*)ptr = 0;

    return ptr;
}

void* realloc(void* ptr, size_t new_size) {
    return ptr;
}

void free(void* ptr) {
    __alloc_blocks[((uintptr_t)ptr) >> ALLOC_BLOCK_SHIFT].free = 0;
}

int atoi(const char *s) {
	int n=0, neg=0;

	while (isspace(*s)) s++;

	switch (*s) {
        case '-': neg=1;
        case '+': s++;
	}
	/* Compute n as a negative number to avoid overflow on INT_MIN */
	while (isdigit(*s))
		n = 10*n - (*s++ - '0');

	return neg ? n : -n;
}

#include "limits.h"
#include "ctype.h"
#include "errno.h"
#include "stdlib.h"

unsigned long strtoul(const char* nptr, char** endptr, int base) {
	register const char *s = nptr;
	register unsigned long acc;
	register int c;
	register unsigned long cutoff;
	register int neg = 0, any, cutlim;

	/*
	 * See strtol for comments as to the logic used.
	 */
	do {
		c = *s++;
	} while (isspace(c));
	if (c == '-') {
		neg = 1;
		c = *s++;
	} else if (c == '+')
		c = *s++;
	if ((base == 0 || base == 16) &&
	    c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0)
		base = c == '0' ? 8 : 10;
	cutoff = (unsigned long)ULONG_MAX / (unsigned long)base;
	cutlim = (unsigned long)ULONG_MAX % (unsigned long)base;
	for (acc = 0, any = 0;; c = *s++) {
		if (isdigit(c))
			c -= '0';
		else if (isalpha(c))
			c -= isupper(c) ? 'A' - 10 : 'a' - 10;
		else
			break;
		if (c >= base)
			break;
		if (any < 0 || acc > cutoff || acc == cutoff && c > cutlim)
			any = -1;
		else {
			any = 1;
			acc *= base;
			acc += c;
		}
	}
	if (any < 0) {
		acc = ULONG_MAX;
		errno = ERANGE;
	} else if (neg)
		acc = -acc;
	if (endptr != 0)
		*endptr = (char *)(any ? s - 1 : nptr);
	return (acc);
}