#ifndef QUATERNION_H
#define QUATERNION_H

struct Quaternion {
	float32 r;
	float32 i;
	float32 j;
	float32 k;

	explicit Quaternion(float32 r, float32 i, float32 j, float32 k) : r(r), i(i), j(j), k(k) { }
	explicit Quaternion() : Quaternion(1, 0, 0, 0) { }
};

#endif // QUATERNION_H
