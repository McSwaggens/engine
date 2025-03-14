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

typedef   float  f32;
typedef   double f64;

typedef char byte;
#define null nullptr

static void ZeroMemory(void* p, u64 size) { __builtin_memset(p, 0, size); }
static void MoveMemory(void* dest, void* src, u64 size) { __builtin_memmove(dest, src, size); }
static void CopyMemory(void* dest, const void* src, u64 size) { __builtin_memcpy(dest, src, size); }
static bool CompareMemory(const void* a, const void* b, u64 size) { return __builtin_memcmp(a, b, size) == 0; }
static void SetMemory(void* p, char c, u32 size) { __builtin_memset(p, c, size); }

#define Trap() __builtin_debugtrap()

#endif // GENERAL_H
