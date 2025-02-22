#include "print.h"
#include "assert.h"

static void Write(OutputBuffer* buffer, char c) { buffer->Write(c); }
static void Write(OutputBuffer* buffer, unsigned long int n) { Write(buffer, (u64)n); }

// 3 5 10 20
// Lut for n < 256?
static void Write(OutputBuffer* buffer, u64 n) {
	const int max = 20; // ceil(log10(pow(2, sizeof(n)*8-1)))
	char digits[max];
	int count = 0;

	do {
		digits[max - count - 1] = '0' + n % 10;
	} while (++count < max && (n /= 10));

	buffer->Write(digits + (max - count), count);
}

static void Write(OutputBuffer* buffer, s64 n) {
	if (n < 0) { buffer->Write('-'); n = -n; }
	Write(buffer, (u64)n);
}

static void Write(OutputBuffer* buffer, void* p) { Write(buffer, Hex((u64)p)); }

static void WriteBase16(OutputBuffer* buffer, u64 n) {
	const u8 length_table[65] = {
		16, 16, 16, 16,
		15, 15, 15, 15,
		14, 14, 14, 14,
		13, 13, 13, 13,
		12, 12, 12, 12,
		11, 11, 11, 11,
		10, 10, 10, 10,
		9,  9,  9,  9,
		8,  8,  8,  8,
		7,  7,  7,  7,
		6,  6,  6,  6,
		5,  5,  5,  5,
		4,  4,  4,  4,
		3,  3,  3,  3,
		2,  2,  2,  2,
		1,  1,  1,  1, 1
	};

	char character_buffer[17];

	u64 digits = length_table[Clz64(n)];
	u64 k = digits << 2;

	for (u64 i = 0; i < digits; i++) {
		k -= 4;
		character_buffer[i] = "0123456789ABCDEF"[(n >> k) & 0xF];
	}

	character_buffer[digits] = 'h';
	buffer->Write(character_buffer, digits+1);
}

static void WriteBase2(OutputBuffer* buffer, u64 n) {
	if (!n) {
		buffer->Write("0b", 2);
		return;
	}

	const u32 table[16] = {
		0x30303030,
		0x31303030,
		0x30313030,
		0x31313030,
		0x30303130,
		0x31303130,
		0x30313130,
		0x31313130,
		0x30303031,
		0x31303031,
		0x30313031,
		0x31313031,
		0x30303131,
		0x31303131,
		0x30313131,
		0x31313131,
	};

	char character_buffer[65];
	s64 lz = Clz64(n);

	for (s64 i = 0; i < 16; i++) {
		((u32*)character_buffer)[i] = table[(n >> (60-(i*4))) & 0x0f];
	}

	character_buffer[64] = 'b';
	buffer->Write(character_buffer+lz, 65-lz);
}

static void Write(OutputBuffer* buffer, IntFormat format) {
	switch (format.base) {
		case BASE_2:  WriteBase2(buffer, format.value);  break;
		case BASE_10: Write(buffer, format.value);       break;
		case BASE_16: WriteBase16(buffer, format.value); break;
	}
}

static void Write(OutputBuffer* buffer, String str) {
	if (!str.data)
		return;

	buffer->Write(str.data, str.length);
}

static void Write(OutputBuffer* buffer, f32 f) {
	Write(buffer, (f64)f);
}

static void Write(OutputBuffer* buffer, f64 f) {
	// @FixMe: This really isn't that great, but it's good enough for now.
	Write(buffer, (s64)f);
	buffer->Write('.');
	Write(buffer, (s64)Abs((f-(s64)f) * Pow(10, 9)));
}

