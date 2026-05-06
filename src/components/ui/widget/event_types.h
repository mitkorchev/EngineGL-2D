#pragma once

class Input;

enum class EventType {
	CLOSE_WINDOW,
	SET_FOCUS_ON_INPUT
};

struct CloseWindowEvent {
	unsigned int targetWindowID;
};

struct SetInputFocusEvent {
	Input* targetInputPointer;
};

struct Event {
	EventType type;

	union {
		CloseWindowEvent closeWindowEvent;
		SetInputFocusEvent setInputFocusEvent;
	};
};