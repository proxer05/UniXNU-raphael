#ifndef _STDDEF_H
#define _STDDEF_H
typedef __typeof__(sizeof(0)) size_t;
typedef long ptrdiff_t;
#define NULL ((void *)0)
#define offsetof(t, m) __builtin_offsetof(t, m)
#endif
