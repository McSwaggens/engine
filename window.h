#ifndef WINDOW_H_INCLUDED
#define WINDOW_H_INCLUDED

#include "general.h"
#include "vector.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

struct Window {
	GLFWwindow* glfw_window;

	void Destroy();
	void Update();
	void SwapBuffers();

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
