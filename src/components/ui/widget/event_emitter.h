#pragma once

#include "event_types.h"

#include <vector>

class EventEmitter {

	std::vector<Event> m_Events;

public:

	EventEmitter() {}

	void PushEvent(Event event) {
		m_Events.emplace_back(event);
	}

	std::vector<Event>& GetEvents() { return m_Events; }

};