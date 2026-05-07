#include "input_controller.h"

std::u32string InputController::m_BufferedUtfInput;

void UtfCharacterInputCallback(GLFWwindow* win, unsigned int input) {
	InputController::AddCharacterToUtfInput(input);
}




bool GameInput::AccessBitmask(
	int _index,
	const std::bitset<MAXIMUM_BITSET_SIZE> &_bitmask
) const {
	return _bitmask[_index];
}


bool GameInput::AccessPreviousKeystateBitmask(
	int _index
) const {
	return AccessBitmask(_index, m_PreviousKeystateBitmask);
}


bool GameInput::AccessRecentKeystateBitmask(
	int _index
) const {
	return AccessBitmask(_index, m_RecentKeystateBitmask);
}

bool GameInput::IsPressed(
	GLuint _openglKeyCode
) const {
	DEBUG_ASSERT(_openglKeyCode >= 0 && _openglKeyCode <= MAXIMUM_TRACKED_KEYS_INDEX,
		"Attempt to check out-of-bounds keystate of keycode [%d].", _openglKeyCode);
	bool isCaptured = false;
	
	if (_openglKeyCode <= GLFW_MOUSE_BUTTON_LAST) {
		isCaptured = m_MouseCaptured;
	}
	else {
		isCaptured = m_KeyboardCaptured;
	}

	return
		!isCaptured &&
		AccessRecentKeystateBitmask(_openglKeyCode) &&
		!AccessPreviousKeystateBitmask(_openglKeyCode);
}


bool GameInput::IsHeld(
	GLuint _openglKeyCode
) const {
	DEBUG_ASSERT(_openglKeyCode >= 0 && _openglKeyCode <= MAXIMUM_TRACKED_KEYS_INDEX,
		"Attempt to check out-of-bounds keystate of keycode [%d].", _openglKeyCode);
	bool isCaptured = false;

	if (_openglKeyCode <= GLFW_MOUSE_BUTTON_LAST) {
		isCaptured = m_MouseCaptured;
	}
	else {
		isCaptured = m_KeyboardCaptured;
	}

	return
		!isCaptured &&
		AccessRecentKeystateBitmask(_openglKeyCode) &&
		AccessPreviousKeystateBitmask(_openglKeyCode);
}


bool GameInput::IsReleased(
	GLuint _openglKeyCode
) const {
	DEBUG_ASSERT(_openglKeyCode >= 0 && _openglKeyCode <= MAXIMUM_TRACKED_KEYS_INDEX,
		"Attempt to check out-of-bounds keystate of keycode [%d].", _openglKeyCode);
	bool isCaptured = false;

	if (_openglKeyCode <= GLFW_MOUSE_BUTTON_LAST) {
		isCaptured = m_MouseCaptured;
	}
	else {
		isCaptured = m_KeyboardCaptured;
	}

	return
		!isCaptured &&
		!AccessRecentKeystateBitmask(_openglKeyCode) &&
		AccessPreviousKeystateBitmask(_openglKeyCode);
}

bool GameInput::IsDown(
	GLuint _openglKeyCode
) const {
	return AccessRecentKeystateBitmask(_openglKeyCode);
}

bool GameInput::IsUp(
	GLuint _openglKeyCode
) const {
	return !AccessRecentKeystateBitmask(_openglKeyCode);
}

void GameInput::SetKeyboardCapturedFlag() {
	m_KeyboardCaptured = true;
}

void GameInput::SetMouseCapturedFlag() {
	m_MouseCaptured = true;
}

void GameInput::ClearCapturedFlags() {
	m_KeyboardCaptured = false;
	m_MouseCaptured = false;
}

void GameInput::GetMousePosition(
	float& OUT_xMouseCoord,
	float& OUT_yMouseCoord
) const {
	OUT_xMouseCoord = m_RecentMouseCoords.x;
	OUT_yMouseCoord = m_RecentMouseCoords.y;
}

glm::vec2 GameInput::GetMousePosition() const {
	return m_RecentMouseCoords;
}

glm::vec2 GameInput::GetMouseChange() const {
	return m_RecentMouseCoords - m_PreviousMouseCoords;
}

void GameInput::SetBufferedInput(
	std::u32string&& MOV_string
) {
	m_BufferedUtfInput = std::move(MOV_string);
}





void InputController::AddCharacterToUtfInput(
	char32_t ch
) {
	m_BufferedUtfInput += ch;
}

void InputController::ClearBufferedUtfInput() {
	m_BufferedUtfInput.clear();
}

InputController::InputController(
	GLFWwindow* _mainWindowHandle
) :
	m_MainWinHandle(_mainWindowHandle)
{
	glfwSetCharCallback(m_MainWinHandle, UtfCharacterInputCallback);
}

void InputController::CaptureKeystates() {

	DEBUG_ASSERT(m_MainWinHandle != nullptr, "InputController used when Window handle is null.");

	m_Input.ClearCapturedFlags();

	m_Input.m_PreviousKeystateBitmask = m_Input.m_RecentKeystateBitmask;
	
	for (int i = 32; i < MAXIMUM_TRACKED_KEYS_INDEX; i++) {
		if(m_TrackedKeystatesBitmask.test(i))
			m_Input.m_RecentKeystateBitmask[i] = glfwGetKey(m_MainWinHandle, i);
	}

	for (int i = 0; i <= GLFW_MOUSE_BUTTON_LAST; ++i)
	{
		m_Input.m_RecentKeystateBitmask[i] =
			(glfwGetMouseButton(m_MainWinHandle, i) == GLFW_PRESS);
	}

	//	Mouse coords

	double xMousePosition, yMousePosition;
	glfwGetCursorPos(m_MainWinHandle, &xMousePosition, &yMousePosition);

	m_Input.m_PreviousMouseCoords = m_Input.m_RecentMouseCoords;
	m_Input.m_RecentMouseCoords = glm::vec2(static_cast<float>(xMousePosition), static_cast<float>(yMousePosition));

	m_Input.SetBufferedInput(std::move(m_BufferedUtfInput));
}

void InputController::SetTrackedKeystatesBitmask(
	TrackingBit _trackingBitmask
) {
	m_TrackedKeystatesBitmask.reset();

	if (_trackingBitmask & c_LetterTrackBit) {
		AddIndicesToTrackedKeystatesBitmask(
			c_LetterKeystateIndices,
			sizeof(c_LetterKeystateIndices) / sizeof(int)
		);
	}

	if (_trackingBitmask & c_DigitTrackBit) {
		AddIndicesToTrackedKeystatesBitmask(
			c_DigitKeystateIndices,
			sizeof(c_DigitKeystateIndices) / sizeof(int)
		);
	}

	if (_trackingBitmask & c_ArrowTrackBit) {
		AddIndicesToTrackedKeystatesBitmask(
			c_ArrowKeystateIndices,
			sizeof(c_ArrowKeystateIndices) / sizeof(int)
		);
	}

	if (_trackingBitmask & c_SpecialSymbolTrackBit) {
		AddIndicesToTrackedKeystatesBitmask(
			c_SpecialSymbolKeystateIndices,
			sizeof(c_SpecialSymbolKeystateIndices) / sizeof(int)
		);
	}

	if (_trackingBitmask & c_FunctionTrackBit) {
		AddIndicesToTrackedKeystatesBitmask(
			c_FunctionKeystateIndices,
			sizeof(c_FunctionKeystateIndices) / sizeof(int)
		);
	}

	if (_trackingBitmask & c_SpecialTrackBit ) {
		AddIndicesToTrackedKeystatesBitmask(
			c_SpecialKeystateIndices,
			sizeof(c_SpecialKeystateIndices) / sizeof(int)
		);
	}
}


void InputController::AddIndicesToTrackedKeystatesBitmask(
	const int* _indexArray,
	int _indexElementCount
) {
	for (int i = 0; i < _indexElementCount; i++) {
		m_TrackedKeystatesBitmask.set( _indexArray[i] );
	}
}