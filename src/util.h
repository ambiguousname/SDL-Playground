#pragma once
#include <vector>
template <typename T>
struct Event {
	void (*callback)(T);
};

template <typename T>
struct EventListener {
	std::vector<Event<T>> listeners;

	void invoke(T arg1) {
		for (auto l : listeners) {
			l(arg1);
		}
	}

	void subscribe(Event e) {
		listeners.push_back(e);
	}
};