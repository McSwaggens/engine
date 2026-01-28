#ifndef MOUSE_H
#define MOUSE_H

#include "vector.h"
#include "engine.h"
#include <GLFW/glfw3.h>

enum class MouseButton {
	Left,
	Right,
	Middle,
	Button4,
	Button5,
	Button6,
	Button7,
	Button8,
};

namespace Mouse {

	static Vector2 position;
	static Vector2 prev_position;
	static Vector2 movement;

	static bool prev_button_state[8] = {};
	static bool curr_button_state[8] = {};

	static bool is_over_window = false;
	static bool entered_window = false;
	static bool exited_window = false;

	static void CursorEnterCallback(GLFWwindow* window, int entered) {
		is_over_window = entered;
		entered_window = entered;
		exited_window = !entered;
	}

	static s32 ToGLFWButton(MouseButton button) {
		switch (button) {
			case MouseButton::Left:    return GLFW_MOUSE_BUTTON_LEFT;
			case MouseButton::Right:   return GLFW_MOUSE_BUTTON_RIGHT;
			case MouseButton::Middle:  return GLFW_MOUSE_BUTTON_MIDDLE;
			case MouseButton::Button4: return GLFW_MOUSE_BUTTON_4;
			case MouseButton::Button5: return GLFW_MOUSE_BUTTON_5;
			case MouseButton::Button6: return GLFW_MOUSE_BUTTON_6;
			case MouseButton::Button7: return GLFW_MOUSE_BUTTON_7;
			case MouseButton::Button8: return GLFW_MOUSE_BUTTON_8;
		}
		return GLFW_MOUSE_BUTTON_LEFT;
	}

	static void Init() {
		glfwSetCursorEnterCallback(Engine::window.glfw_window, CursorEnterCallback);
	}

	static void Update() {
		entered_window = false;
		exited_window = false;

		f64 x, y;
		glfwGetCursorPos(Engine::window.glfw_window, &x, &y);

		prev_position = position;
		position = Vector2((f32)x, (f32)y);
		movement = position - prev_position;

		for (s32 i = 0; i < 8; i++) {
			prev_button_state[i] = curr_button_state[i];
			curr_button_state[i] = glfwGetMouseButton(Engine::window.glfw_window, i) == GLFW_PRESS;
		}
	}

	static bool IsDown(MouseButton button) {
		s32 glfw_button = ToGLFWButton(button);
		return curr_button_state[glfw_button];
	}

	static bool IsUp(MouseButton button) {
		return !IsDown(button);
	}

	static bool IsPressed(MouseButton button) {
		s32 glfw_button = ToGLFWButton(button);
		return curr_button_state[glfw_button] && !prev_button_state[glfw_button];
	}

	static bool IsReleased(MouseButton button) {
		s32 glfw_button = ToGLFWButton(button);
		return !curr_button_state[glfw_button] && prev_button_state[glfw_button];
	}

	static void SetLocked(bool locked) {
		glfwSetInputMode(Engine::window.glfw_window, GLFW_CURSOR,
			locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	}

	static void SetPosition(Vector2 pos) {
		glfwSetCursorPos(Engine::window.glfw_window, (f64)pos.x, (f64)pos.y);
		position = pos;
		prev_position = pos;
		movement = Vector2(0, 0);
	}

}

#endif // MOUSE_H
