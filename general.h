#ifndef GENERAL_H
#define GENERAL_H

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

typedef   signed char      s8;
typedef   signed short     s16;
typedef   signed int       s32;
typedef   signed long long s64;

typedef   float float32;
typedef   double float64;

typedef char byte;
#define null nullptr

static void ZeroMemory(void* p, u64 size) { __builtin_memset(p, 0, size); }
static void MoveMemory(void* dest, void* src, u64 size) { __builtin_memmove(dest, src, size); }
static void CopyMemory(void* dest, void* src, u64 size) { __builtin_memcpy(dest, src, size); }
static bool CompareMemory(void* a, void* b, u64 size) { return __builtin_memcmp(a, b, size) == 0; }

#endif // GENERAL_H
