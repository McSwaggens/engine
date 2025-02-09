#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "general.h"
#include "string.h"
#include "array.h"
#include "os.h"

typedef s32 DirectoryHandle;

struct File {
	FileHandle handle;

	explicit File(FileHandle handle) : handle(handle) { }

	bool IsValid() { return handle != -1; }
	void Write(const char* src, u64 size);
	void Read(char* dest, u64 length);
	u64  QueryFileSize();
	void Close();
};

static const u64 OUTPUT_BUFFER_SIZE = 4096 * 2;

struct OutputBuffer {
	File file;
	u32  head;
	byte buffer[OUTPUT_BUFFER_SIZE];

	void Write(const char* data, u64 size) {
		Assert(head < OUTPUT_BUFFER_SIZE);

		if (head + size < OUTPUT_BUFFER_SIZE) {
			CopyMemory(buffer + head, data, size);
			head += size;
			return;
		}

		file.Write(buffer, head);
		head = 0;

		if (size > OUTPUT_BUFFER_SIZE) {
			file.Write(data, size);
			head = 0;
			return;
		}

		CopyMemory(buffer, data, size);
		head = size;
	}

	void Write(char c) {
		Assert(head < OUTPUT_BUFFER_SIZE);

		buffer[head++] = c;

		if (head == OUTPUT_BUFFER_SIZE)
			Flush();
	}

	void Flush() {
		if (head == 0)
			return;

		file.Write(buffer, head);
		head = 0;
	}
};

static OutputBuffer standard_output_buffer { .file = File(STDOUT), .head = 0 };
static OutputBuffer standard_error_buffer  { .file = File(STDERR), .head = 0 };

enum FileMode
{
	FILE_MODE_OPEN,     // Open an existing file.
	FILE_MODE_APPEND,   // Open an existing file and go to the end.
	FILE_MODE_TRUNCATE, // Open and truncate an existing file.
	FILE_MODE_CREATE,   // Create a file that doesn't already exist.
						// ProTip: Believe it or not; there is an 'e' at the end of 'create'!

	FILE_MODE_CREATE_OR_OPEN,     // Open or create a file.
	FILE_MODE_CREATE_OR_APPEND,   // Create file if it doesn't already exist, otherwise go to the end.
	FILE_MODE_CREATE_OR_TRUNCATE, // Truncate an existing file, otherwise create one.
};


typedef u32 FileAccessFlags;
static const FileAccessFlags FILE_ACCESS_READ  = 0x1;
static const FileAccessFlags FILE_ACCESS_WRITE = 0x2;

static File        OpenFile(String path);
static Array<byte> LoadFile(String path);
static bool        DoesFileExist(String path);

#endif // FILE_SYSTEM_H
