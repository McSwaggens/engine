#ifndef WINDOW_H_INCLUDED
#define WINDOW_H_INCLUDED

#include "general.h"
#include "vector.h"

struct Window {
	GLFWwindow* glfw_window;
	VkSurfaceKHR surface;

	u32 width  = 0;
	u32 height = 0;

	bool has_size_changed = false;

	void Destroy();
	void Update();
	void SwapBuffers();
	bool InitSurface();

	void UpdateSize();

	Vector2 GetPosition();
	Vector2 GetSize();
	Vector2 GetFrameBufferSize();

	void SetPosition(Vector2 pos);
	void SetSize(Vector2 size);

	void Show();
	void Hide();

	bool ShouldClose();
};

static Window CreateWindow();

#endif // WINDOW_H_INCLUDED
