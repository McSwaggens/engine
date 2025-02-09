#ifndef LIST_H
#define LIST_H

#include "general.h"
#include "assert.h"

template<typename T>
struct List {
	T* elements;
	u32 length;
	u32 capacity;

	List(T* elements, u32 length, u32 capacity) :
		elements(elements), length(length), capacity(capacity) { }

	T& operator[](u32 n) {
		Assert(n < length);
		return elements[n];
	}

	T* operator (T*)() { return elements; }

	void Add(T t) {
		elements = ReAlloc(length+1 * sizeof(T));
		elements[length++] = t;
	}

	void Pop(u32 n = 1) {
		// elements = ReAlloc(elements, length * sizeof(T) (length-n) * sizeof(T));
		length -= n;
	}

	void Free() {
		Free(elements, sizeof(T) * length);
	}
};

#endif // LIST_H
