#ifndef _STDINT_H
#define _STDINT_H
typedef signed char int8_t; typedef unsigned char uint8_t;
typedef short int16_t; typedef unsigned short uint16_t;
typedef int int32_t; typedef unsigned int uint32_t;
typedef long long int64_t; typedef unsigned long long uint64_t;
typedef unsigned long uintptr_t; typedef long intptr_t;
typedef unsigned long u_long; typedef unsigned int u_int;
typedef unsigned short u_short; typedef unsigned char u_char;
typedef long ssize_t;
#define INT32_MAX 0x7fffffff
#define UINT32_MAX 0xffffffffu
#define UINT64_MAX 0xffffffffffffffffull
#define SIZE_MAX ((size_t)-1)
#endif
