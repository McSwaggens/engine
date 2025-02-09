#ifndef QUATERNION_H
#define QUATERNION_H

struct Quaternion {
	f32 r;
	f32 i;
	f32 j;
	f32 k;

	explicit Quaternion(f32 r, f32 i, f32 j, f32 k) : r(r), i(i), j(j), k(k) { }
	explicit Quaternion() : Quaternion(1, 0, 0, 0) { }
};

#endif // QUATERNION_H
