#ifndef LIST_H
#define LIST_H

#include "general.h"
#include "assert.h"
#include "alloc.h"

template<typename T>
struct List {
	T* elements;
	u32 count;
	u32 capacity;

	explicit List() :
		elements(0), count(0), capacity(0) { }

	explicit List(T* elements, u32 count, u32 capacity) :
		elements(elements), count(count), capacity(capacity) { }

	T& operator[](u32 n) {
		Assert(n < count);
		return elements[n];
	}

	operator T*() { return elements; }

	void Add(T t) {
		elements = (T*)ReAllocMemory(elements, count * sizeof(T), count+1 * sizeof(T));
		elements[count++] = t;
	}

	void Pop(u32 n = 1) {
		// elements = ReAlloc(elements, count * sizeof(T) (count-n) * sizeof(T));
		count -= n;
	}

	void Free() {
		Free(elements, sizeof(T) * count);
	}
};

#endif // LIST_H
