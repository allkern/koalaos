#include "string.h"
#include "stdint.h"
#include "stddef.h"

void* memset(void* dst, int c, unsigned int n) {
    char* ptr = (char*)dst;

    while (n--)
        *ptr++ = c;

    return dst;
}

int memcmp(const void* v1, const void* v2, unsigned int n) {
    const unsigned char *s1, *s2;

    s1 = v1;
    s2 = v2;

    while (n-- > 0) {
        if (*s1 != *s2)
            return *s1 - *s2;

        s1++, s2++;
    }

    return 0;
}

void* memmove(void* dst, const void* src, unsigned int n) {
    const char* s;
    char* d;

    if (n == 0)
        return dst;
    
    s = src;
    d = dst;

    if (s < d && s + n > d) {
        s += n;
        d += n;

        while (n-- > 0)
            *--d = *--s;
    } else {
        while (n-- > 0)
            *d++ = *s++;
    }

    return dst;
}

void* memcpy(void* dst, const void* src, unsigned int n) {
    return memmove(dst, src, n);
}

int strcmp(const char* s1, const char* s2) {
	while (*s1 == *s2++)
		if (*s1++ == '\0')
			return 0;

	return (*(const unsigned char*)s1 - *(const unsigned char*)(s2 - 1));
}

int strncmp(const char* p, const char* q, unsigned int n) {
    while (n > 0 && *p && *p == *q)
        n--, p++, q++;

    if (n == 0)
        return 0;

    return (unsigned char)*p - (unsigned char)*q;
}

char* strncpy(char* s, const char* t, int n) {
    char* os;

    os = s;

    while (n-- > 0 && (*s++ = *t++) != 0);

    while (n-- > 0)
        *s++ = 0;

    return os;
}

char* s_strcpy(char* s, const char* t, int n) {
    char* os;

    os = s;

    if (n <= 0)
        return os;

    while (--n > 0 && (*s++ = *t++) != 0);

    *s = 0;

    return os;
}

int strlen(const char* s) {
    int n;

    for (n = 0; s[n]; n++);

    return n;
}

char* strchr(char* str, char c) {
    while (*str) {
        if (*str == c)
            return str;

        ++str; 
    }

    return str;
}

char* strrchr(char* str, char c) {
    char* end = strchr(str, '\0');

    while (end != str) {
        if (*end == c)
            return end;

        --end;
    }

    return end;
}