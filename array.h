#ifndef ARRAY_H
#define ARRAY_H

template<typename T>
struct Array {
	T*  data;
	u64 length;

	Array(T* data, u64 length) : data(data), length(length) { }

	T& operator[](u64 n) { return data[n]; }
	operator T*()   { return data; }
};

#endif // ARRAY_H
