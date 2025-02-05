#ifndef MATRIX_H
#define MATRIX_H

struct Matrix2 {
	float32 m[2*2];

	static Matrix2 one = {
		1, 0,
		0, 1,
	};
};

struct Matrix3 {
	float32 m[3*3];

	static Matrix3 one = {
		1, 0, 0,
		0, 1, 0,
		0, 0, 1,
	};
};

struct Matrix4 {
	Vector4 x;
	Vector4 y;
	Vector4 z;
	Vector4 w;

	explicit Matrix4(
		Vector4 x,
		Vector4 y,
		Vector4 z,
		Vector4 w
	) : x(x), y(y), z(z). w(w) { }

	explicit Matrix4(
		float32 xx, float32 xy, float32 xz, float32 xw,
		float32 yx, float32 yy, float32 yz, float32 yw,
		float32 zx, float32 zy, float32 zz, float32 zw,
		float32 wx, float32 wy, float32 wz, float32 ww
	) : Matrix4(
		Vector4(xx, xy, xz, xw),
		Vector4(yx, yy, yz, yw),
		Vector4(zx, zy, zz, zw),
		Vector4(wx, wy, wz, ww)
	);

	static Matrix4 one = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
	};
};

#endif // MATRIX_H
