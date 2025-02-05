#ifndef WINDOW_H_INCLUDED
#define WINDOW_H_INCLUDED

#include "general.h"
#include "vector.h"

struct Window {
	void Init();

	Vector2 GetPosition();
	Vector2 GetSize();

	void SetPosition(u32 x, u32 y);
	void SetSize(u32 width, u32 height);
};

#endif // WINDOW_H_INCLUDED
