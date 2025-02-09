#ifndef STRING_H
#define STRING_H

#include "alloc.h"
#include "assert.h"

struct String {
	char* data   = 0;
	u32 length   = 0;
	u32 capacity = 0;

	String() = default;
	template<u32 N>
	String(const char (&str)[N]) : data(const_cast<char*>(str)), length(N-1), capacity(0) { }
	explicit String(const char* str, u32 length, u32 capacity) : data(const_cast<char*>(str)), length(length), capacity(capacity) { }

	void Free() {
		Assert(capacity || !length);
		FreeMemory(data, capacity); 
	}

	String Copy() {
		return String((char*)CopyAllocMemory(data, length), length, length);
	}

	void ExportCString(char* cstr) {
		CopyMemory(cstr, data, length);
		cstr[length] = 0;
	}

	void ResizeToFit(u32 count) {
		if (length + count < capacity)
			return;

		capacity = NextPow2((length+count) | 15);
		data = (char*)ReAllocMemory(data, length, capacity);
	}

	void Add(char c) {
		Assert(capacity|| !length);
		ResizeToFit(1);
		data[length++] = c;
	}

	void Add(String str) {
		Assert(capacity|| !length);
		ResizeToFit(str.length);
		CopyMemory(data + length, str.data, str.length);
		length += str.length;
	}

	void Prepend(char c) {
		ResizeToFit(1); // @OptimizeMe Copy + Move -> Copy.
		MoveMemory(data, data+1, length);
		data[0] = c;
	}
};

static u64 CStringLength(const char* str) {
	const char* p = str;
	while (*p) p++;
	return p - str;
}

static String FromCString(const char* str) {
	return String(str, CStringLength(str), 0);
}

#endif // STRING_H
