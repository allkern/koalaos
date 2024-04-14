#include "stdio.h"
#include "stdarg.h"
#include "ctype.h"

struct STDIO {
    int (*read)(void);
    void (*write)(int);
} __stdio;

int stdio_read(void* udata, void* buf, fpos_t pos, size_t size) {
    struct STDIO* stdio = (struct STDIO*)udata;
    char* ptr = (char*)buf;

    // pos is unused for stdio
    (void)pos;

    while (size--)
        *ptr++ = stdio->read();

    return 0;
}

int stdio_write(void* udata, void* buf, fpos_t pos, size_t size) {
    struct STDIO* stdio = (struct STDIO*)udata;
    char* ptr = (char*)buf;

    // pos is unused for stdio
    (void)pos;

    while (size--)
        stdio->write(*ptr++);

    return 0;
}

int stdio_seek(void* udata, fpos_t pos) {
    return 0;
}

static FILE __stdin, __stdout;

void __libc_init_stdio(int (*read)(void), void (*write)(int)) {
    __stdio.read = read;
    __stdio.write = write;

    __stdin.eof = 0;
    __stdin.err = 0;
    __stdin.mode = IOR;
    __stdin.pos = 0;
    __stdin.udata = &__stdio;
    __stdin.read = stdio_read;
    __stdin.seek = stdio_seek;
    __stdin.write = NULL;

    __stdout.eof = 0;
    __stdout.err = 0;
    __stdout.mode = IOW;
    __stdout.pos = 0;
    __stdout.udata = &__stdio;
    __stdout.read = NULL;
    __stdout.seek = stdio_seek;
    __stdout.write = stdio_write;

    stdin = &__stdin;
    stdout = &__stdout;
    stderr = &__stdout;
}

// int printf(const char* fmt, ...) {
//     va_list args;

//     va_start(args, fmt);

//     int n = vfprintf(stdout, fmt, args);

//     va_end(args);

//     return n;
// }

// int sprintf(char* buf, const char* fmt, ...);
int snprintf(char* buf, size_t size, const char* fmt, ...);
int getc(FILE* file) {
    if (!(file->mode & IOR))
        return 0;
    
    char buf;

    file->read(file->udata, &buf, file->pos++, sizeof(char));

    return buf;
}

int getchar(void) {
    return getc(stdin);
}

char* gets(char* buf) {
    char c = getchar();

    while (c) {
        *buf++ = c;

        c = getchar();
    }

    return buf;
}

char* gets_s(char* buf, size_t count) {
    char c = getchar();

    while (c && (count--)) {
        *buf++ = c;

        c = getchar();
    }

    return buf;
}
int putc(int c, FILE* file) {
    fputc(c, file);
}

int putchar(int c) {
    fputc(c, stdout);
}

int puts(const char* str) {
    fputs(str, stdout);
}

int ungetc(int count, FILE* file) {
    file->pos -= count;

    return count;
}

void perror(const char* str) {
    // To-do: Print error based on errno

    printf("%s: Error", str);
}

FILE* fopen(const char* filename, const char* mode);
int fseek(FILE* file, long offset, int origin);
long ftell(FILE* file);
int feof(FILE* file);
int ferror(FILE* file);
size_t fread(void* buf, size_t size, size_t count, FILE* file);
size_t fwrite(const void* buf, size_t size, size_t count, FILE* file);
int fgetc(FILE* file);
char* fgets(char* buf, int count, FILE* file);

int fputc(int c, FILE* file) {
    if (!(file->mode & IOW))
        return 0;
    
    char buf = c;

    file->write(file->udata, &buf, file->pos++, sizeof(char));
}

int fputs(const char* str, FILE* file) {
    int i = 0;

    while (str[i])
        fputc(str[i++], file);

    fputc('\n', file);

    return i;
}

void print_string(FILE* file, const char* str) {
    while (*str)
        fputc(*str++, file);
}

static char digits[] = "0123456789abcdef";

void print_int(FILE* file, int n, int base, int sign) {
    char buf[16];
    unsigned int x;

    if (sign && (sign = n < 0)) {
        x = -n;
    } else {
        x = n;
    }

    int i = 0;

    do {
        buf[i++] = digits[x % base];
    } while ((x /= base) != 0);

    if (sign)
        buf[i++] = '-';

    while(--i >= 0)
        fputc(buf[i], file);
}

void print_ptr(FILE* file, uintptr_t ptr) {
    for (int i = 0; i < (sizeof(uintptr_t) * 2); i++, ptr <<= 4)
        fputc(digits[ptr >> (sizeof(uintptr_t) * 8 - 4)], file);
}

int vfprintf(FILE* file, const char* fmt, va_list args) {
    while (*fmt != '\0') {
        if (*fmt == '%') {
            char f = *(++fmt);

            if (f == 's') {
                const char* s = __builtin_va_arg(args, const char*);

                print_string(file, s);
            } else if (f == 'x') {
                unsigned int x = __builtin_va_arg(args, unsigned int);

                print_int(file, x, 16, 0);
            } else if (f == 'd') {
                int x = __builtin_va_arg(args, int);

                print_int(file, x, 10, 1);
            } else if (f == 'u') {
                unsigned int x = __builtin_va_arg(args, unsigned int);

                print_int(file, x, 10, 0);
            } else if (f == 'i') {
                int x = __builtin_va_arg(args, int);

                print_int(file, x, 10, 1);
            } else if (f == 'p') {
                uintptr_t p = __builtin_va_arg(args, uintptr_t);

                print_ptr(file, p);
            } else if (f == 'c') {
                int c = __builtin_va_arg(args, int);

                fputc(c, file);
            } else if (f == '%') {
                fputc('%', file);
            } else {
                return 0;
            }
        } else {
            fputc(*fmt, file);
        }

        ++fmt;
    }

    return 1;
}

int fprintf(FILE* file, const char* fmt, ...) {
    va_list args;

    va_start(args, fmt);

    int n = vfprintf(file, fmt, args);

    va_end(args);

    return n;
}