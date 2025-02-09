#include "os.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

static void* AllocPages(u64 size) {
	size = size+(PAGE_SIZE-1) & -PAGE_SIZE;
	return mmap(null, size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, 0, 0);
}

static void FreePages(void* p, u64 size) {
	munmap(p, size);
}

