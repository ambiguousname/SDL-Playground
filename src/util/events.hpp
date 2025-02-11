#pragma once
#include "macros.hpp"
#include <vector>

template <typename T>
struct Event {
	void (*callback)(T data);
};

template <typename T>
class EventListener {
	protected:
	std::vector<Event<T>> listeners;
	public:
	void invoke(T data) {
		for (auto l: listeners) {
			l.invoke(data);
		}
	}

	void subscribe(Event<T> e) {
		listeners.push_back(e);
	}
};