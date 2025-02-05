#ifndef OS_H
#define OS_H

#include "general.h"
#include "string.h"

#define MACOS 1

#if MACOS
	#define PAGE_SIZE (16 << 10)
#else
	#define PAGE_SIZE (4 << 10)
#endif

typedef u32 FileHandle;

static const FileHandle STDIN  = 0;
static const FileHandle STDOUT = 1;
static const FileHandle STDERR = 2;

byte* AllocPages(u64 size);
void  FreePages(void* p, u64 size);

FileHandle OpenFile(String path);
void CloseFile(FileHandle file);

#endif // OS_H
