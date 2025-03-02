#ifndef WINDOW_H_INCLUDED
#define WINDOW_H_INCLUDED

#include "general.h"
#include "vector.h"

struct Window {
	GLFWwindow* glfw_window;
	VkSurfaceKHR surface;

	void Destroy();
	void Update();
	void SwapBuffers();
	bool InitSurface();

	Vector2 GetPosition();
	Vector2 GetSize();

	void SetPosition(Vector2 pos);
	void SetSize(Vector2 size);

	void Show();
	void Hide();

	bool ShouldClose();
};

static Window CreateWindow();

#endif // WINDOW_H_INCLUDED
