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

	List() : elements(0), count(0), capacity(0) { }

	explicit List(T* elements, u32 count, u32 capacity) :
		elements(elements), count(count), capacity(capacity) { }

	template<u32 N>
	List(const T (&a)[N]) : List(static_cast<T*>(a), N, 0) { }

	T& operator[](u32 n) {
		Assert(n < count);
		return elements[n];
	}

	operator T*() { return elements; }

	// For for-loop iteration.
	T* begin() { return elements; }
	T* end()   { return elements + count; }

	T* Begin() { return elements; }
	T* End()   { return elements + count; }

	void AssureCapacity(u32 new_capacity) {
		if (capacity >= new_capacity)
			return;

		new_capacity = RoundPow2(new_capacity);
		elements = (T*)ReAllocMemory(elements, capacity * sizeof(T), new_capacity * sizeof(T));
		capacity = new_capacity;
	}

	void AssureCount(u32 new_count) {
		AssureCapacity(new_count);
		count = new_count;
	}

	void Add(T t) {
		AssureCapacity(count+1);
		elements[count++] = t;
	}

	void Add(List<T> list) {
		AssureCapacity(count + list.count);
		CopyMemory(elements + count, list.elements, list.count * sizeof(T));
		count += list.count;
	}

	void Insert(T value, u32 index) {
		AssureCapacity(count+1);
		MoveMemory(elements + index, elements + index + 1, (count - index) * sizeof(T));
		elements[index] = value;
		count++;
	}

	void Remove(u32 begin, u32 end) {
		Assert(begin < end);
		Assert(end < count);

		MoveMemory(elements + end, elements + begin, (count - end) * sizeof(T));
		count -= end - begin;
	}

	void Remove(u32 index) {
		Remove(index, index + 1);
	}

	void RemoveAll(T value) {
		T* r = elements;

		// Find first occurance.
		for (; *r != value; r++);

		T* l = r;

		// Start 
		while (r < End()) {
			if (*r == value) {
				r++;
				continue;
			}

			*l++ = *r++;
		}

		count = l - elements;
	}

	void RemoveDuplicates() {
		T* l = elements;
		T* r = elements;

		while (r < End()) {
		}
	}

	void Pop(u32 n = 1) {
		Assert(count >= n);
		count -= n;
	}

	void Free() {
		FreeMemory(elements, sizeof(T) * count);
	}

};

#endif // LIST_H
