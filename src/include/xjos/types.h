#ifndef ONIX_TYPES_H
#define ONIX_TYPES_H

#include <xjos/xjos.h>

#define EOF (-1) // END OF FILE
#define EOS '\0'  // END OF STRING

#define NULL ((void*)0) // NULL pointer

#define bool _Bool
#define true (1)
#define false (0)

#define _packed __attribute__((packed))  // * disable byte alignment 

#define _ofp __attribute__((optimize("omit-frame-pointer")))

typedef unsigned int size_t;

typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef u32 time_t;

#endif