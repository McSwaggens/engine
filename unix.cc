#include "os.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <stdlib.h>

static void* AllocPages(u64 size) {
	size = size+(PAGE_SIZE-1) & -PAGE_SIZE;
	return mmap(null, size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, 0, 0);
}

static void FreePages(void* p, u64 size) {
	munmap(p, size);
}

static u64 GetTimeMicroseconds() {
	timeval tv;
	gettimeofday(&tv, null);
	u64 seconds = tv.tv_sec * 1000000llu;
	u64 micros  = tv.tv_usec;
	return seconds + micros;
}

static void ExitProgram() {
	exit(0);
}
