#ifndef PRINT_H
#define PRINT_H

#include "general.h"
#include "file_system.h"
#include "string.h"

#define Log(var) Print("%:%: note: " #var " = %\n", CString(__FILE__), __LINE__, var)

struct OutputBuffer;

enum Base {
	BASE_2,
	BASE_10,
	BASE_16,
};

struct IntFormat {
	Base base;
	u64  value;
};

static inline IntFormat Bin(u64 n) { return (IntFormat){ .base = BASE_2,  .value = n }; }
static inline IntFormat Hex(u64 n) { return (IntFormat){ .base = BASE_16, .value = n }; }
static void Write(OutputBuffer* buffer, IntFormat format);
static void Write(OutputBuffer* buffer, char c);
static void Write(OutputBuffer* buffer, u64  n);
static void Write(OutputBuffer* buffer, s64  n);
static void Write(OutputBuffer* buffer, f32  n);
static void Write(OutputBuffer* buffer, f64  n);
static void Write(OutputBuffer* buffer, unsigned long int n); // Need this, otherwise sizeof won't work...
static void Write(OutputBuffer* buffer, void* p);
static void Write(OutputBuffer* buffer, String str);

static void Write(OutputBuffer* buffer, struct Vector2 v);
static void Write(OutputBuffer* buffer, struct Vector3 v);
static void Write(OutputBuffer* buffer, struct Vector4 v);
static void Write(OutputBuffer* buffer, struct Quaternion q);

// Need these for disambiguation.
static void Write(OutputBuffer* buffer, u8  n) { Write(buffer, (u64)n); }
static void Write(OutputBuffer* buffer, u16 n) { Write(buffer, (u64)n); }
static void Write(OutputBuffer* buffer, u32 n) { Write(buffer, (u64)n); }
static void Write(OutputBuffer* buffer, s8  n) { Write(buffer, (s64)n); }
static void Write(OutputBuffer* buffer, s16 n) { Write(buffer, (s64)n); }
static void Write(OutputBuffer* buffer, s32 n) { Write(buffer, (s64)n); }

template<typename T>
static void Write(OutputBuffer* buffer, Array<T> array) {
	buffer->Write("{ ", 2);

	for (u64 i = 0; i < array.count; i++) {
		if (i != 0) buffer->Write(", ", 2);
		Write(buffer, array[i]);
	}

	buffer->Write(" }", 2);
}

template<typename ...Args>
static void Print(OutputBuffer* buffer, String format, Args&&... args) {
	char* end = format.data + format.length;
	char* p = format.data;

	auto internal_print = [=, &p]<typename T>(T&& t) {
		char* start = p;

		while (p < end && *p != '%') p++;

		if (start != p)
			buffer->Write(start, p-start);

		if (p < end)
		{
			Write(buffer, t);
			p++;
		}
	};

	(internal_print(args),...);

	if (p < end)
		buffer->Write(p, end - p);
}

template<typename ...Args>
static void Print(String format, Args&&... args) {
	Print(&standard_output_buffer, format, args...);
}

static void Write(OutputBuffer* buffer, bool b) {
	if (b) buffer->Write("true",  4);
	else   buffer->Write("false", 5);
}


#endif // PRINT_H
