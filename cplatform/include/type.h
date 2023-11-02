#ifndef TYPE_H
#define TYPE_H

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <wchar.h>

typedef int8_t s8;
typedef uint8_t u8;
typedef int16_t s16;
typedef uint16_t u16;
typedef int32_t s32;
typedef uint32_t u32;
typedef int64_t s64;
typedef uint64_t u64;

typedef s8 sbyte;
typedef u8 byte;
typedef s16 sword;
typedef u16 word;
typedef s32 sdword;
typedef u32 dword;
typedef s64 sqword;
typedef u64 qword;

typedef wchar_t wchar;

typedef unsigned int uint;

typedef unsigned char bool;

#define UINT8_MIN  0
#define UINT16_MIN 0
#define UINT32_MIN 0
#define UINT64_MIN 0

#define S8_MIN	((s8)INT8_MIN)
#define S8_MAX	((s8)INT8_MAX)
#define U8_MIN	((u8)UINT8_MIN)
#define U8_MAX	((u8)UINT8_MAX)
#define S16_MIN ((s16)INT16_MIN)
#define S16_MAX ((s16)INT16_MAX)
#define U16_MIN ((u16)UINT16_MIN)
#define U16_MAX ((u16)UINT16_MAX)
#define S32_MIN ((s32)INT32_MIN)
#define S32_MAX ((s32)INT32_MAX)
#define U32_MIN ((u32)UINT32_MIN)
#define U32_MAX ((u32)UINT32_MAX)
#define S64_MIN ((s64)INT64_MIN)
#define S64_MAX ((s64)INT64_MAX)
#define U64_MIN ((u64)UINT64_MIN)
#define U64_MAX ((u64)UINT64_MAX)

#endif
