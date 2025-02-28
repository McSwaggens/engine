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

	List() :
		elements(0), count(0), capacity(0) { }

	explicit List(T* elements, u32 count, u32 capacity) :
		elements(elements), count(count), capacity(capacity) { }

	template<u32 N>
	List(const T (&a)[N]) : List(static_cast<T*>(a), N, 0) { }

	T& operator[](u32 n) {
		Assert(n < count);
		return elements[n];
	}

	operator T*() { return elements; }

	void AssureCapacity(u32 new_capacity)
	{
		if (capacity < new_capacity)
		{
			new_capacity = RoundPow2(new_capacity);
			elements = (T*)ReAllocMemory(elements, capacity * sizeof(T), new_capacity * sizeof(T));
			capacity = new_capacity;
		}
	}

	void AssureCount(u32 new_count)
	{
		AssureCapacity(new_count);
		count = new_count;
	}

	void Add(T t) {
		AssureCapacity(count+1);
		elements[count++] = t;
	}

	void Add(List<T> list)
	{
		AssureCapacity(count + list.count);
		CopyMemory(elements + count, list.elements, list.count * sizeof(T));
		count += list.count;
	}

	void Pop(u32 n = 1) {
		Assert(count >= n);
		count -= n;
	}

	void Free() {
		FreeMemory(elements, sizeof(T) * count);
	}


	// For for-loop iteration.
	T* begin() { return elements; }
	T* end()   { return elements + count; }

	T* Begin() { return elements; }
	T* End()   { return elements + count; }
};

#endif // LIST_H
