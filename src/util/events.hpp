#pragma once
#include "macros.hpp"
#include <vector>

#define _TEMPLATE_EXPAND(arg) typename arg,
#define _TEMPLATE_EXPAND_STOP(arg) typename arg 

#define EVENT_TEMPLATE_EXPAND(...) FOR_EACH(_TEMPLATE_EXPAND, _TEMPLATE_EXPAND_STOP, ##__VA_ARGS__)

// Should be <T, void, void, void, void> for something basic.
//#define EVENT_TEMPLATE_FULL(...) _FOR_EACH_EXPAND_4(_FULL_EXPAND, _FULL_EXPAND_STOP, ##__VA_ARGS__, void, void, void, void)

#define _CONSTRUCT_ARGS_EXPAND(a) a arg ## a,
#define _CONSTRUCT_ARGS_STOP(a) a arg ## a

#define CONSTRUCT_ARGS_EXPAND(...) FOR_EACH(_CONSTRUCT_ARGS_EXPAND, _CONSTRUCT_ARGS_STOP, ##__VA_ARGS__)

#define _CALL_ARGS_EXPAND(a) arg ## a,
#define _CALL_ARGS_STOP(a) arg ## a

#define CALL_ARGS_EXPAND(...) FOR_EACH(_CALL_ARGS_EXPAND, _CALL_ARGS_STOP, ##__VA_ARGS__)

template <typename T, typename U = void, typename V = void, typename W = void, typename X = void>
struct Event {
	void (*callback)(T, U, V, W, X);
};

template <typename T, typename U = void, typename V = void, typename W = void, typename X = void>
class EventListener {
	protected:
	std::vector<Event<T, U, V, W, X>> listeners;
	public:
	void invoke(T arg1, U arg2, V arg3, W arg4, X arg5) {
		for (auto l: listeners) {
			l.invoke(arg1, arg2, arg3, arg4, arg5);
		}
	}

	void subscribe(Event<T, U, V, W, X> e) {
		listeners.push_back(e);
	}
};

#define EVENT(...) \
template <EVENT_TEMPLATE_EXPAND(__VA_ARGS__)> \
struct Event<__VA_ARGS__> { \
	void (*callback)(__VA_ARGS__); \
}; \
template <EVENT_TEMPLATE_EXPAND(__VA_ARGS__)> \
class EventListener<__VA_ARGS__> { \
	protected: \
	std::vector<Event<__VA_ARGS__>> listeners;\
	public: \
	void invoke(CONSTRUCT_ARGS_EXPAND(__VA_ARGS__)) {\
		for (auto l: listeners) {\
			(*l.callback)(CALL_ARGS_EXPAND(__VA_ARGS__));\
		}\
	}\
	void subscribe(Event<__VA_ARGS__> e) {\
		listeners.push_back(e);\
	}\
};

EVENT(T)
EVENT(T, U)
EVENT(T, U, V)
EVENT(T, U, V, W)