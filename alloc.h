#ifndef ALLOC_H
#define ALLOC_H

#include "general.h"

static InitGlobalAllocator();
static byte* AllocMemory(u64 size);
static void FreeMemory(byte* p);
static byte* ReAllocMemory(byte* p, u64 old_size, u64 new_size);
static byte* CopyAllocMemory(byte* p, u64 size);

template<typename T>
static T* Alloc(u64 count = 1) { return AllocMemory(sizeof(T) * count); }

template<typename T>
static void Free(T* p, u64 count = 1) { FreeMemory(p, sizeof(T) * count); }

#endif // ALLOC_H
