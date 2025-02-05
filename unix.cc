#include "os.h"

#include <sys/mman.h>
#include <sys/io.h>

byte* AllocPages(u64 size) {
	size = size+(PAGE_SIZE-1) & -PAGE_SIZE;
	return mmap(null, size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, 0, 0);
}

void  FreePages(void* p, u64 size) {
	munmap(p, size);
}

FileHandle OpenFile(String path) {
	char cpath[path.length+1];
	Copy(cpath, (char*)path, path.length);
	cpath[path.length] = 0;
	return open(cpath);
}

void CloseFile(FileHandle file) {
	close(file);
}
