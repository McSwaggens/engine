#ifndef ALLOC_H
#define ALLOC_H

#include "general.h"

static void InitGlobalAllocator();
static void* AllocMemory(u64 size);
static void FreeMemory(void* p);
static void* ReAllocMemory(void* p, u64 old_size, u64 new_size);
static void* CopyAllocMemory(void* p, u64 size);

template<typename T>
static T* Alloc(u64 count = 1) { return AllocMemory(sizeof(T) * count); }

template<typename T>
static void Free(T* p, u64 count = 1) { FreeMemory(p, sizeof(T) * count); }

#endif // ALLOC_H
