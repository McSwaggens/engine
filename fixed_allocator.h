#ifndef FIXED_ALLOCATOR_H
#define FIXED_ALLOCATOR_H

#include "general.h"
#include "assert.h"

template<typename T, u32 N>
struct FixedAllocator {
	T stack[N];
	u32 head = 0;

	T* Next() {
		Assert(head < N);
		return &stack[head++];
	}

	T* begin() { return stack; }
	T* end()   { return stack + head; }
};

#endif // FIXED_ALLOCATOR_H
