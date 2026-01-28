#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "general.h"
#include <GLFW/glfw3.h>

#include "engine.h"

enum class Key {
	// Alphabetic keys
	A, B, C, D, E, F, G, H, I, J, K, L, M,
	N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

	// Numeric keys (top row)
	Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,

	// Function keys
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

	// Modifier keys
	LeftShift, RightShift,
	LeftCtrl, RightCtrl,
	LeftAlt, RightAlt,
	LeftSuper, RightSuper,

	// Navigation keys
	Up, Down, Left, Right,
	Home, End,
	PageUp, PageDown,
	Insert, Delete,

	// Editing/Control keys
	Escape, Tab, CapsLock, Space, Enter, Backspace,
	PrintScreen, ScrollLock, Pause, Menu,

	// Punctuation/Symbol keys
	Grave,        // `
	Minus,        // -
	Equals,       // =
	LeftBracket,  // [
	RightBracket, // ]
	Backslash,    // '\'
	Semicolon,    // ;
	Apostrophe,   // '
	Comma,        // ,
	Period,       // .
	Slash,        // /

	// Numpad keys
	Numpad0, Numpad1, Numpad2, Numpad3, Numpad4,
	Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,
	NumpadDecimal,
	NumpadDivide,
	NumpadMultiply,
	NumpadSubtract,
	NumpadAdd,
	NumpadEnter,
	NumpadEqual,
};

namespace Keyboard {

	static bool prev_key_state[512] = {};
	static bool curr_key_state[512] = {};

	static s32 ToGLFWKey(Key key) {
		switch (key) {
			// Alphabetic keys
			case Key::A: return GLFW_KEY_A;
			case Key::B: return GLFW_KEY_B;
			case Key::C: return GLFW_KEY_C;
			case Key::D: return GLFW_KEY_D;
			case Key::E: return GLFW_KEY_E;
			case Key::F: return GLFW_KEY_F;
			case Key::G: return GLFW_KEY_G;
			case Key::H: return GLFW_KEY_H;
			case Key::I: return GLFW_KEY_I;
			case Key::J: return GLFW_KEY_J;
			case Key::K: return GLFW_KEY_K;
			case Key::L: return GLFW_KEY_L;
			case Key::M: return GLFW_KEY_M;
			case Key::N: return GLFW_KEY_N;
			case Key::O: return GLFW_KEY_O;
			case Key::P: return GLFW_KEY_P;
			case Key::Q: return GLFW_KEY_Q;
			case Key::R: return GLFW_KEY_R;
			case Key::S: return GLFW_KEY_S;
			case Key::T: return GLFW_KEY_T;
			case Key::U: return GLFW_KEY_U;
			case Key::V: return GLFW_KEY_V;
			case Key::W: return GLFW_KEY_W;
			case Key::X: return GLFW_KEY_X;
			case Key::Y: return GLFW_KEY_Y;
			case Key::Z: return GLFW_KEY_Z;

			// Numeric keys (top row)
			case Key::Num0: return GLFW_KEY_0;
			case Key::Num1: return GLFW_KEY_1;
			case Key::Num2: return GLFW_KEY_2;
			case Key::Num3: return GLFW_KEY_3;
			case Key::Num4: return GLFW_KEY_4;
			case Key::Num5: return GLFW_KEY_5;
			case Key::Num6: return GLFW_KEY_6;
			case Key::Num7: return GLFW_KEY_7;
			case Key::Num8: return GLFW_KEY_8;
			case Key::Num9: return GLFW_KEY_9;

			// Function keys
			case Key::F1:  return GLFW_KEY_F1;
			case Key::F2:  return GLFW_KEY_F2;
			case Key::F3:  return GLFW_KEY_F3;
			case Key::F4:  return GLFW_KEY_F4;
			case Key::F5:  return GLFW_KEY_F5;
			case Key::F6:  return GLFW_KEY_F6;
			case Key::F7:  return GLFW_KEY_F7;
			case Key::F8:  return GLFW_KEY_F8;
			case Key::F9:  return GLFW_KEY_F9;
			case Key::F10: return GLFW_KEY_F10;
			case Key::F11: return GLFW_KEY_F11;
			case Key::F12: return GLFW_KEY_F12;

			// Modifier keys
			case Key::LeftShift:  return GLFW_KEY_LEFT_SHIFT;
			case Key::RightShift: return GLFW_KEY_RIGHT_SHIFT;
			case Key::LeftCtrl:   return GLFW_KEY_LEFT_CONTROL;
			case Key::RightCtrl:  return GLFW_KEY_RIGHT_CONTROL;
			case Key::LeftAlt:    return GLFW_KEY_LEFT_ALT;
			case Key::RightAlt:   return GLFW_KEY_RIGHT_ALT;
			case Key::LeftSuper:  return GLFW_KEY_LEFT_SUPER;
			case Key::RightSuper: return GLFW_KEY_RIGHT_SUPER;

			// Navigation keys
			case Key::Up:       return GLFW_KEY_UP;
			case Key::Down:     return GLFW_KEY_DOWN;
			case Key::Left:     return GLFW_KEY_LEFT;
			case Key::Right:    return GLFW_KEY_RIGHT;
			case Key::Home:     return GLFW_KEY_HOME;
			case Key::End:      return GLFW_KEY_END;
			case Key::PageUp:   return GLFW_KEY_PAGE_UP;
			case Key::PageDown: return GLFW_KEY_PAGE_DOWN;
			case Key::Insert:   return GLFW_KEY_INSERT;
			case Key::Delete:   return GLFW_KEY_DELETE;

			// Editing/Control keys
			case Key::Escape:      return GLFW_KEY_ESCAPE;
			case Key::Tab:         return GLFW_KEY_TAB;
			case Key::CapsLock:    return GLFW_KEY_CAPS_LOCK;
			case Key::Space:       return GLFW_KEY_SPACE;
			case Key::Enter:       return GLFW_KEY_ENTER;
			case Key::Backspace:   return GLFW_KEY_BACKSPACE;
			case Key::PrintScreen: return GLFW_KEY_PRINT_SCREEN;
			case Key::ScrollLock:  return GLFW_KEY_SCROLL_LOCK;
			case Key::Pause:       return GLFW_KEY_PAUSE;
			case Key::Menu:        return GLFW_KEY_MENU;

			// Punctuation/Symbol keys
			case Key::Grave:        return GLFW_KEY_GRAVE_ACCENT;
			case Key::Minus:        return GLFW_KEY_MINUS;
			case Key::Equals:       return GLFW_KEY_EQUAL;
			case Key::LeftBracket:  return GLFW_KEY_LEFT_BRACKET;
			case Key::RightBracket: return GLFW_KEY_RIGHT_BRACKET;
			case Key::Backslash:    return GLFW_KEY_BACKSLASH;
			case Key::Semicolon:    return GLFW_KEY_SEMICOLON;
			case Key::Apostrophe:   return GLFW_KEY_APOSTROPHE;
			case Key::Comma:        return GLFW_KEY_COMMA;
			case Key::Period:       return GLFW_KEY_PERIOD;
			case Key::Slash:        return GLFW_KEY_SLASH;

			// Numpad keys
			case Key::Numpad0: return GLFW_KEY_KP_0;
			case Key::Numpad1: return GLFW_KEY_KP_1;
			case Key::Numpad2: return GLFW_KEY_KP_2;
			case Key::Numpad3: return GLFW_KEY_KP_3;
			case Key::Numpad4: return GLFW_KEY_KP_4;
			case Key::Numpad5: return GLFW_KEY_KP_5;
			case Key::Numpad6: return GLFW_KEY_KP_6;
			case Key::Numpad7: return GLFW_KEY_KP_7;
			case Key::Numpad8: return GLFW_KEY_KP_8;
			case Key::Numpad9: return GLFW_KEY_KP_9;
			case Key::NumpadDecimal:  return GLFW_KEY_KP_DECIMAL;
			case Key::NumpadDivide:   return GLFW_KEY_KP_DIVIDE;
			case Key::NumpadMultiply: return GLFW_KEY_KP_MULTIPLY;
			case Key::NumpadSubtract: return GLFW_KEY_KP_SUBTRACT;
			case Key::NumpadAdd:      return GLFW_KEY_KP_ADD;
			case Key::NumpadEnter:    return GLFW_KEY_KP_ENTER;
			case Key::NumpadEqual:    return GLFW_KEY_KP_EQUAL;
		}
		return GLFW_KEY_UNKNOWN;
	}

	static void Update() {
		for (s32 i = 0; i < 512; i++) {
			prev_key_state[i] = curr_key_state[i];
			curr_key_state[i] = glfwGetKey(Engine::window.glfw_window, i) == GLFW_PRESS;
		}
	}

	static bool IsDown(Key key) {
		s32 glfw_key = ToGLFWKey(key);
		return curr_key_state[glfw_key];
	}

	static bool IsUp(Key key) {
		return !IsDown(key);
	}

	static bool IsPressed(Key key) {
		s32 glfw_key = ToGLFWKey(key);
		return curr_key_state[glfw_key] && !prev_key_state[glfw_key];
	}

	static bool IsReleased(Key key) {
		s32 glfw_key = ToGLFWKey(key);
		return !curr_key_state[glfw_key] && prev_key_state[glfw_key];
	}

}

#endif // KEYBOARD_H
