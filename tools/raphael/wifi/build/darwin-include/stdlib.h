#ifndef _STDLIB_H
#define _STDLIB_H
#include <stddef.h>
void *malloc(size_t);
void *calloc(size_t, size_t);
void *realloc(void *, size_t);
void free(void *);
void exit(int) __attribute__((noreturn));
void abort(void) __attribute__((noreturn));
int atoi(const char *);
long strtol(const char *, char **, int);
unsigned long strtoul(const char *, char **, int);
int abs(int);
char *getenv(const char *);
void qsort(void *, size_t, size_t, int (*)(const void *, const void *));
void arc4random_buf(void *, size_t);
unsigned long long strtoull(const char *, char **, int);
long long strtoll(const char *, char **, int);
#endif
