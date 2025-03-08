#include "window.h"
	Vector2 GetFrameBufferSize();

static bool InitWindowSystem() {
	if (!glfwInit()) return false;
	Assert(glfwVulkanSupported());
	return true;
}

static Window CreateWindow() {
	Window window = { };
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window.glfw_window = glfwCreateWindow(640, 480, "Engine", null, null);
	return window;
}

void Window::Destroy() {
	vkDestroySurfaceKHR(vk, surface, null);
	glfwDestroyWindow(glfw_window);
	glfw_window = null;
}

void Window::Update() {
	glfwPollEvents();
}

bool Window::InitSurface() {
	return glfwCreateWindowSurface(vk, glfw_window, null, &surface);
}

Vector2 Window::GetPosition() {
	s32 x, y;
	glfwGetWindowPos(glfw_window, &x, &y);
	return Vector2(x, y);
}

Vector2 Window::GetSize() {
	s32 width, height;
	glfwGetWindowSize(glfw_window, &width, &height);
	return Vector2(width, height);
}

Vector2 Window::GetFrameBufferSize() {
	s32 width, height;
	glfwGetFramebufferSize(glfw_window, &width, &height);
	return Vector2(width, height);
}

void Window::SetPosition(Vector2 pos) { glfwSetWindowPos(glfw_window, pos.x, pos.y); }
void Window::SetSize(Vector2 size)    { glfwSetWindowSize(glfw_window, size.x, size.y); }
bool Window::ShouldClose() { return glfwWindowShouldClose(glfw_window); }
void Window::SwapBuffers() { glfwSwapBuffers(glfw_window); }
void Window::Show() { glfwShowWindow(glfw_window); }
void Window::Hide() { glfwHideWindow(glfw_window); }

