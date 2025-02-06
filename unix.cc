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

static FileHandle OpenFile(String path) {
	char cpath[path.length+1];
	CopyMemory(cpath, (char*)path.data, path.length);
	cpath[path.length] = 0;
	return open(cpath, 0);
}

static void CloseFile(FileHandle file) {
	close(file);
}
