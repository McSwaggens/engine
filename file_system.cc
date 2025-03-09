#include "file_system.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static File OpenFile(String path) {
	char cpath[path.length+1];
	path.ExportCString(cpath);

	FileHandle handle = open(cpath, O_RDWR | O_APPEND);
	return File(handle);
}

void File::Write(const char* src, u64 size) {
	write(handle, src, size);
}

void File::Read(char* dest, u64 length) {
	read(handle, dest, length);
}

u64 File::QueryFileSize() {
	struct stat status;
	fstat(handle, &status);
	return status.st_size;
}

void File::Close() {
	close(handle);
}

static bool DoesFileExist(String path)
{
	char cpath[path.length+1];
	path.ExportCString(cpath);
	// F = 0, X = 1, W = 2, R = 4
	return access(cpath, 0) == 0;
}

static Array<byte> LoadFile(String path) {
	File file = OpenFile(path);
	u64 size = file.QueryFileSize();

	byte* p = (byte*)AllocMemory(size);
	file.Read(p, size);
	file.Close();

	Array<byte> result = {
		.data   = p,
		.length = size,
	};

	return result;
}
