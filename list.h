#ifndef LIST_H
#define LIST_H

#include "general.h"

template<typename T>
struct List {
	u32 length;
	T* elements;

	T& operator[](u32 n) { return elements[n]; }

	void Add(T t) {
		elements = ReAlloc(length+1 * sizeof(T));
		elements[length++] = t;
	}

	void Pop(u32 n = 1) {
		elements = ReAlloc(elements, length * sizeof(T) (length-n) * sizeof(T));
		length -= n;
	}

	void Free() {
		Free(elements, sizeof(T) * length);
	}
};

#endif // LIST_H
