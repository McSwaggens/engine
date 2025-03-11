#ifndef OS_H
#define OS_H

#include "general.h"
#include "string.h"

#ifdef MACOS
	#define PAGE_SIZE (16 << 10)
#else
	#define PAGE_SIZE (4 << 10)
#endif

typedef s32 FileHandle;

static const FileHandle STDIN  = 0;
static const FileHandle STDOUT = 1;
static const FileHandle STDERR = 2;

static void* AllocPages(u64 size);
static void  FreePages(void* p, u64 size);

static u64 GetTimeMicroseconds();

static void ExitProgram();

#endif // OS_H
