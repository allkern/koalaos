#include "limits.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "errno.h"
#include "stdio.h"

#include "sys/dmem.h"

#define ATEXIT_MAX 32

void (*__atexit_func_array[ATEXIT_MAX])(void);

static int __atexit_func_index;

void __libc_init_stdlib(void) {
	for (int i = 0; i < ATEXIT_MAX; i++)
		__atexit_func_array[i] = NULL;

	__atexit_func_index = 0;
}

int atexit(void (*func)(void)) {
    __atexit_func_array[__atexit_func_index++] = func;
}

void exit(int code) {
    for (int i = 0; i < __atexit_func_index && i < ATEXIT_MAX; i++)
        __atexit_func_array[i]();
}

void* malloc(size_t size) {
	return dmem_alloc(size);
}

void* calloc(size_t num, size_t size) {
    size_t s = num * size;

    void* ptr = dmem_alloc(s);

    for (int i = 0; i < s; i++)
        *(unsigned char*)ptr = 0;

    return ptr;
}

void* realloc(void* ptr, size_t new_size) {
	// realloc allows passing in NULL pointers, for some reason.
	// in which case it's the same as malloc(new_size)
	if (!ptr)
		return dmem_alloc(new_size);

	// Try to extend directly, avoids having to copy the data
	// over to a new buffer
	if (dmem_extend(ptr, new_size))
		return ptr;

	// If not, try allocating a buffer of size new_size and moving
	// the data to the new buffer
	void* buf = dmem_alloc(new_size);

	// If not, fail
	if (!buf)
		return NULL;

	// Move the data over to the new buffer
	memcpy(buf, ptr, dmem_get_alloc_size(ptr));

	// Free the original buffer
	dmem_free(ptr);

    return buf;
}

void free(void* ptr) {
	dmem_free(ptr);
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