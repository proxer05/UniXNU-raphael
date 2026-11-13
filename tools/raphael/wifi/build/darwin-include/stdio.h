#ifndef _STDIO_H
#define _STDIO_H
#include <stddef.h>
typedef struct _opaque FILE;
extern FILE *__stdoutp;
#define stdout __stdoutp
extern FILE *__stderrp;
#define stderr __stderrp
int printf(const char *, ...);
int fprintf(FILE *, const char *, ...);
int snprintf(char *, size_t, const char *, ...);
int sprintf(char *, const char *, ...);
int vprintf(const char *, __builtin_va_list);
int sscanf(const char *, const char *, ...);
FILE *fopen(const char *, const char *);
int fclose(FILE *);
size_t fread(void *, size_t, size_t, FILE *);
size_t fwrite(const void *, size_t, size_t, FILE *);
int fseek(FILE *, long, int);
long ftell(FILE *);
char *fgets(char *, int, FILE *);
int fputs(const char *, FILE *);
int fputc(int, FILE *);
int fflush(FILE *);
int feof(FILE *);
int ferror(FILE *);
void perror(const char *);
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
int fscanf(FILE *, const char *, ...);
#define EOF (-1)
#endif
FILE *freopen(const char *, const char *, FILE *);
int fileno(FILE *);
int setvbuf(FILE *, char *, int, size_t);
int remove(const char *);
#define _IOFBF 0
#define _IOLBF 1
#define _IONBF 2
int putchar(int);
int getchar(void);
