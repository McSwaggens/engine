#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED

#include "general.h"
#include "math.h"

struct Vector2 {
	float32 x;
	float32 y;

	Vector2(float32 x, float32 y) : x(x), y(y) { }
	Vector2(float32 f) : Vector2(f, f) { }

	float32 Dot(Vector2 v) { return x*v.x + y*v.y; }
	float32 Length()       { return Sqrt(x*x + y*y); }

	Vector2 operator +(Vector2 v) { return Vector2(x + v.x, y + v.y); }
	Vector2 operator -(Vector2 v) { return Vector2(x - v.x, y - v.y); }
	Vector2 operator *(Vector2 v) { return Vector2(x * v.x, y * v.y); }
	Vector2 operator /(Vector2 v) { return Vector2(x / v.x, y / v.y); }

	Vector2 operator +(float32 f) { return Vector2(x + f, y + f); }
	Vector2 operator -(float32 f) { return Vector2(x - f, y - f); }
	Vector2 operator *(float32 f) { return Vector2(x * f, y * f); }
	Vector2 operator /(float32 f) { return Vector2(x / f, y / f); }

	Vector2 Normal() { return *this / Vector2(Length()); }
	// Vector2 Cross(Vector2 v) { return Normal(); }
};

struct Vector3 {
	float32 x;
	float32 y;
	float32 z;

	Vector3(float32 x, float32 y, float32 z) : x(x), y(y), z(z) { }
	Vector3(Vector2 v, float32 z) : Vector3(v.x, v.y, z) { }
	Vector3(float32 f) : Vector3(f, f, f) { }
	Vector3() : Vector3(0, 0, 0) { }

	explicit operator Vector2() { return Vector2(x, y); }

	float32 Dot(Vector3 v) { return x*v.x + y*v.y + z*v.z; }
	float32 Length() { return Sqrt(x*x + y*y + z*z); }

	Vector3 operator +(Vector3 v) { return Vector3(x + v.x, y + v.y, z + v.z); }
	Vector3 operator -(Vector3 v) { return Vector3(x - v.x, y - v.y, z - v.z); }
	Vector3 operator *(Vector3 v) { return Vector3(x * v.x, y * v.y, z * v.z); }
	Vector3 operator /(Vector3 v) { return Vector3(x / v.x, y / v.y, z / v.z); }

	Vector3 operator +(float32 f) { return Vector3(x + f, y + f, z + f); }
	Vector3 operator -(float32 f) { return Vector3(x - f, y - f, z - f); }
	Vector3 operator *(float32 f) { return Vector3(x * f, y * f, z * f); }
	Vector3 operator /(float32 f) { return Vector3(x / f, y / f, z / f); }
};

struct Vector4 {
	float32 x;
	float32 y;
	float32 z;
	float32 w;

	explicit operator Vector2() { return Vector2(x, y); }
	explicit operator Vector3() { return Vector3(x, y, z); }

	float32 Dot(Vector4 v) { return x*v.x + y*v.y + z*v.z + w*v.w; }
	float32 Length() { return Sqrt(x*x + y*y + z*z + w*w); }

	Vector4 operator +(Vector4 v) { return Vector4(x + v.x, y + v.y, z + v.z, w + v.w); }
	Vector4 operator -(Vector4 v) { return Vector4(x - v.x, y - v.y, z - v.z, w - v.w); }
	Vector4 operator *(Vector4 v) { return Vector4(x * v.x, y * v.y, z * v.z, w * v.w); }
	Vector4 operator /(Vector4 v) { return Vector4(x / v.x, y / v.y, z / v.z, w / v.w); }

	Vector4 operator +(float32 f) { return Vector4(x + f, y + f, z + f, w + f); }
	Vector4 operator -(float32 f) { return Vector4(x - f, y - f, z - f, w - f); }
	Vector4 operator *(float32 f) { return Vector4(x * f, y * f, z * f, w * f); }
	Vector4 operator /(float32 f) { return Vector4(x / f, y / f, z / f, w / f); }
};

static float32 Dot(Vector2 a, Vector2 b) { return a.Dot(b); }
static float32 Dot(Vector3 a, Vector3 b) { return a.Dot(b); }
static float32 Dot(Vector4 a, Vector4 b) { return a.Dot(b); }

#endif // VECTOR_H_INCLUDED
