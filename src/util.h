#pragma once
#include <vector>

enum ErrorKind {
	SDL,
	OTHER
};
void logErr(ErrorKind kind, std::string message);

#define GET_N_ARGS(_1, _2, _3, _4, _5, N, ...) N

#define _EVENT_EXPAND_0(arg, ...) typename arg
#define _EVENT_EXPAND_1(arg, ...) typename arg, _EVENT_EXPAND_0(__VA_ARGS__)
#define _EVENT_EXPAND_2(arg, ...) typename arg, _EVENT_EXPAND_1(##__VA_ARGS__)
#define _EVENT_EXPAND_3(arg, ...) typename arg, _EVENT_EXPAND_2(##__VA_ARGS__)
#define _EVENT_EXPAND_4(arg, ...) typename arg, _EVENT_EXPAND_3(##__VA_ARGS__)
#define _EVENT_EXPAND(arg, ...) \
GET_N_ARGS("ignored", ##__VA_ARGS__, _EVENT_EXPAND_4, _EVENT_EXPAND_3, _EVENT_EXPAND_2, _EVENT_EXPAND_1, _EVENT_EXPAND_0)(arg, ##__VA_ARGS__)

#define _FOR_EACH_EXPAND_0(_call, _call_end, arg) _call_end(arg)
#define _FOR_EACH_EXPAND_1(_call, _call_end, arg, ...) _call(arg) _FOR_EACH_EXPAND_0(_call, _call_end, ##__VA_ARGS__)
#define _FOR_EACH_EXPAND_2(_call, _call_end, arg, ...) _call(arg) _FOR_EACH_EXPAND_1(_call, _call_end, ##__VA_ARGS__)
#define _FOR_EACH_EXPAND_3(_call, _call_end, arg, ...) _call(arg) _FOR_EACH_EXPAND_2(_call, _call_end, ##__VA_ARGS__)
#define _FOR_EACH_EXPAND_4(_call, _call_end, arg, ...) _call(arg) _FOR_EACH_EXPAND_3(_call, _call_end, ##__VA_ARGS__)

#define FOR_EACH(_call, _call_end, arg, ...) \
GET_N_ARGS("ignored", ##__VA_ARGS__, _FOR_EACH_EXPAND_4, _FOR_EACH_EXPAND_3, _FOR_EACH_EXPAND_2, _FOR_EACH_EXPAND_1, _FOR_EACH_EXPAND_0)(_call, _call_end, arg, ##__VA_ARGS__)

#define _TEMPLATE_EXPAND(arg) typename arg,
#define _TEMPLATE_EXPAND_STOP(arg) typename arg 

#define EVENT_TEMPLATE_EXPAND(...) FOR_EACH(_TEMPLATE_EXPAND, _TEMPLATE_EXPAND_STOP, ##__VA_ARGS__)

#define _CONSTRUCT_ARGS_EXPAND(a) a arg ## a,
#define _CONSTRUCT_ARGS_STOP(a) a arg ## a

#define CONSTRUCT_ARGS_EXPAND(...) FOR_EACH(_CONSTRUCT_ARGS_EXPAND, _CONSTRUCT_ARGS_STOP, ##__VA_ARGS__)

#define _CALL_ARGS_EXPAND(a) arg ## a,
#define _CALL_ARGS_STOP(a) arg ## a

#define CALL_ARGS_EXPAND(...) FOR_EACH(_CALL_ARGS_EXPAND, _CALL_ARGS_STOP, ##__VA_ARGS__)

#define EVENT(...) \
template <EVENT_TEMPLATE_EXPAND(##__VA_ARGS__)> \
struct Event { \
	void (*callback)(__VA_ARGS__); \
}; \
template <EVENT_TEMPLATE_EXPAND(##__VA_ARGS__)> \
struct EventListener { \
	std::vector<Event<##__VA_ARGS__>> listeners;\
	void invoke(CONSTRUCT_ARGS_EXPAND(##__VA_ARGS__)) {\
		for (auto l: listeners) {\
			l.invoke(CALL_ARGS_EXPAND(##__VA_ARGS__));\
		}\
	}\
};

EVENT(T)
EVENT(T, U)
EVENT(T, U, V)
EVENT(T, U, V, W)
EVENT(T, U, V, W, X)

template<>
struct Event<void> {
	void (*callback)();
};

struct EventListener<void> {
	std::vector<Event<void>> listeners;
	void invoke () {
		for (auto l : listeners) {
			l.callback();
		}
	}

	void subscribe(Event<void> e) {
		listeners.push_back(e);
	}
};