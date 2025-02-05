#include "alloc.h"

struct GlobalAllocator {
} static ga = { };

static InitGlobalAllocator() {
}

static byte* AllocMemory(u64 size) {
}

static void FreeMemory(byte* p);
static byte* ReAllocMemory(byte* p, u64 old_size, u64 new_size);
static byte* CopyAllocMemory(byte* p, u64 size);


