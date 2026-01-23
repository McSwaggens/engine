#include "image.h"

void* njAllocMem(int size) {
	return AllocMemory(sizeof(int) + size) + sizeof(int);
}

void njFreeMem(void* block) {
	int* p = (int*)block - 1;
	FreeMemory(p, *p);
}

void njFillMem(void* block, unsigned char b, int size) { SetMemory(block, b,   size); }
void njCopyMem(void* dest,  const void* src, int size) { CopyMemory(dest, src, size); }

#include "nanojpeg.c"

static Image* AllocImage(u64 data_size) {
	Image* image = AllocMemory(sizeof(Image) + data_size);
	return image;
}

static Image* LoadImage(String path) {
}

