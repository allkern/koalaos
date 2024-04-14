#ifndef STDIO_H
#define STDIO_H

#include "stdint.h"
#include "stdlib.h"
#include "stdarg.h"

// fpos_t is a non-array complete object type, can be used to
// store (by fgetpos) and restore (by fsetpos) the position and
// multibyte parser state (if any) for a C stream. 
typedef uint32_t fpos_t;

#define EOF (-1)
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define IOR 1
#define IOW 2
#define IOA 4
#define IOB 8

#define BUFSIZE 4096

// Implementation-defined FILE object, this should be
// used as an opaque struct, use the typedef instead.
struct __FILE {
    int mode;
    int eof;
    int err;
    fpos_t pos;

    // Implementation-specific functions
    void* udata;
    int (*read)(void* udata, void* buf, fpos_t pos, size_t size);
    int (*write)(void* udata, void* buf, fpos_t pos, size_t size);
    int (*seek)(void* udata, fpos_t pos);
};

void __libc_init_stdio(int (*stdio_read)(void), void (*stdio_write)(int));

typedef struct __FILE FILE;

static FILE* stdin;
static FILE* stdout;
static FILE* stderr;

int printf(const char* fmt, ...);
int sprintf(char* buf, const char* fmt, ...);
int snprintf(char* buf, size_t size, const char* fmt, ...);
int vprintf(const char* fmt, va_list list);
int vsprintf(char* buf, const char* fmt, va_list list);
int vsnprintf(char* buf, size_t size, const char* fmt, va_list list);
int getc(FILE* file);
int getchar(void);
char* gets(char* buf);
char* gets_s(char* buf, size_t count);
int putc(int c, FILE* file);
int putchar(int c);
int puts(const char* str);
int ungetc(int count, FILE* file);
void perror(const char* str);

// File API

FILE* fopen(const char* filename, const char* mode);
int fseek(FILE* file, long offset, int origin);
long ftell(FILE* file);
int feof(FILE* file);
int ferror(FILE* file);
size_t fread(void* buf, size_t size, size_t count, FILE* file);
size_t fwrite(const void* buf, size_t size, size_t count, FILE* file);
int fgetc(FILE* file);
char* fgets(char* buf, int count, FILE* file);
int fputc(int c, FILE* file);
int fputs(const char* str, FILE* file);
int fprintf(FILE* file, const char* fmt, ...);
int vfprintf(FILE* file, const char* fmt, va_list list);

#endif