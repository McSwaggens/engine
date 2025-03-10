#ifndef ARRAY_H
#define ARRAY_H

template<typename T>
struct Array {
	T*  data = null;
	u64 length = 0;

	T& operator[](u64 n) { return data[n]; }
	operator T*()   { return data; }

	T* begin() { return data; }
	T* end()   { return data + length; }

	void Free() {
		FreeMemory(data, length);
	}
};

#endif // ARRAY_H
