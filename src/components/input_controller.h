#pragma once

#include <bitset>

#include "../common/common.h"
#include "../common/standard_quad.h"

static constexpr int MAXIMUM_TRACKED_KEYS_INDEX = 350;
static constexpr int MAXIMUM_BITSET_SIZE = 512;

typedef unsigned int TrackingBit;

struct GameInput {

	std::bitset<MAXIMUM_BITSET_SIZE> m_RecentKeystateBitmask;
	std::bitset<MAXIMUM_BITSET_SIZE> m_PreviousKeystateBitmask;

	glm::vec2 m_RecentMouseCoords;
	glm::vec2 m_PreviousMouseCoords;

	bool m_KeyboardCaptured = false;
	bool m_MouseCaptured = false;

	std::u32string m_BufferedUtfInput;

public:

	void SetKeyboardCapturedFlag();
	void SetMouseCapturedFlag();
	void ClearCapturedFlags();
	
	void SetBufferedInput(
		std::u32string&& MOV_string
	);

public:

	bool IsReleased(
		GLuint _openglKeyCode
	) const;

	bool IsHeld(
		GLuint _openglKeyCode
	) const;

	bool IsPressed(
		GLuint _openglKeyCode
	) const;

	bool IsDown(
		GLuint _openglKeyCode
	) const;

	bool IsUp(
		GLuint _openglKeyCode
	) const;

	void GetMousePosition(
		float& OUT_xMouseCoord,
		float& OUT_yMouseCoord
	) const;

	glm::vec2 GetMouseChange() const;

	glm::vec2 GetMousePosition() const;

	std::u32string& GetUtfInput() { return m_BufferedUtfInput; }

public:

	bool AccessBitmask(
		int _index,
		const std::bitset<MAXIMUM_BITSET_SIZE>& _bitmask
	) const;


	bool AccessPreviousKeystateBitmask(
		int _index
	) const;


	bool AccessRecentKeystateBitmask(
		int _index
	) const;
};


class InputController {

	GLFWwindow* m_MainWinHandle = nullptr;

	static std::u32string m_BufferedUtfInput;

	GameInput m_Input;
	
private:

	std::bitset<MAXIMUM_BITSET_SIZE> m_TrackedKeystatesBitmask;

	void AddIndicesToTrackedKeystatesBitmask(
		const int* _indexArray,
		int _indexElementCount
	);

	static constexpr int c_LetterKeystateIndices[] = {
		GLFW_KEY_A, GLFW_KEY_B, GLFW_KEY_C, GLFW_KEY_D, GLFW_KEY_E,
		GLFW_KEY_F, GLFW_KEY_G, GLFW_KEY_H, GLFW_KEY_I, GLFW_KEY_J,
		GLFW_KEY_K, GLFW_KEY_L, GLFW_KEY_M, GLFW_KEY_N, GLFW_KEY_O,
		GLFW_KEY_P, GLFW_KEY_Q, GLFW_KEY_R, GLFW_KEY_S, GLFW_KEY_T,
		GLFW_KEY_U, GLFW_KEY_V, GLFW_KEY_W, GLFW_KEY_X, GLFW_KEY_Y, GLFW_KEY_Z
	};

	static constexpr int c_DigitKeystateIndices[] = {
		GLFW_KEY_0, GLFW_KEY_1, GLFW_KEY_2, GLFW_KEY_3, GLFW_KEY_4,
		GLFW_KEY_5, GLFW_KEY_6, GLFW_KEY_7, GLFW_KEY_8, GLFW_KEY_9
	};

	static constexpr int c_ArrowKeystateIndices[] = {
		GLFW_KEY_LEFT, GLFW_KEY_RIGHT, GLFW_KEY_UP, GLFW_KEY_DOWN
	};

	static constexpr int c_SpecialSymbolKeystateIndices[] = {
		GLFW_KEY_SLASH, GLFW_KEY_SEMICOLON, GLFW_KEY_APOSTROPHE
	};

	static constexpr int c_FunctionKeystateIndices[] = {
		GLFW_KEY_F1, GLFW_KEY_F2, GLFW_KEY_F3, GLFW_KEY_F4, GLFW_KEY_F5,
		GLFW_KEY_F6, GLFW_KEY_F7, GLFW_KEY_F8, GLFW_KEY_F9, GLFW_KEY_F10,
		GLFW_KEY_F11, GLFW_KEY_F12
	};

	static constexpr int c_SpecialKeystateIndices[] = {
		GLFW_KEY_SPACE, GLFW_KEY_ENTER, GLFW_KEY_TAB, GLFW_KEY_BACKSPACE,
		GLFW_KEY_ESCAPE, GLFW_KEY_LEFT_SHIFT, GLFW_KEY_RIGHT_SHIFT,
		GLFW_KEY_LEFT_CONTROL, GLFW_KEY_RIGHT_CONTROL,
		GLFW_KEY_LEFT_ALT, GLFW_KEY_RIGHT_ALT
	};

public:

	static constexpr TrackingBit c_LetterTrackBit			= 1u << 0;
	static constexpr TrackingBit c_DigitTrackBit			= 1u << 1;
	static constexpr TrackingBit c_ArrowTrackBit			= 1u << 2;
	static constexpr TrackingBit c_SpecialSymbolTrackBit	= 1u << 3;
	static constexpr TrackingBit c_FunctionTrackBit			= 1u << 4;
	static constexpr TrackingBit c_SpecialTrackBit			= 1u << 5;

public:

	InputController() {}

	InputController(
		GLFWwindow* _mainWindowHandle
	);

	void CaptureKeystates();

	void SetTrackedKeystatesBitmask(
		TrackingBit _trackingBitmask
	);

	static void AddCharacterToUtfInput(
		char32_t ch
	);

	static void ClearBufferedUtfInput();

public:

	GameInput& ExposeGameInput() { return m_Input; }


#if DEBUG__CODE

	void	DEBUG_ForceHoldKeyState(GLuint _openglKeycode) {
		m_Input.m_RecentKeystateBitmask.set(_openglKeycode);
		m_Input.m_PreviousKeystateBitmask.set(_openglKeycode);
	}

	void	DEBUG_ForcePressKeyState(GLuint _openglKeycode) {
		m_Input.m_RecentKeystateBitmask.set(_openglKeycode);
	}

#else

	inline void	DEBUG_ForceHoldKeyState(GLuint _openglKeycode) 
	{}

	inline void	DEBUG_ForcePressKeyState(GLuint _openglKeycode) 
	{}

#endif

};