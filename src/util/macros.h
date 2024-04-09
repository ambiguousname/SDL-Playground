#pragma once

#define GET_N_ARGS(_1, _2, _3, _4, _5, N, ...) N

#define _FOR_EACH_EXPAND_0(_call, _call_end, arg) _call_end(arg)
#define _FOR_EACH_EXPAND_1(_call, _call_end, arg, ...) _call(arg) _FOR_EACH_EXPAND_0(_call, _call_end, ##__VA_ARGS__)
#define _FOR_EACH_EXPAND_2(_call, _call_end, arg, ...) _call(arg) _FOR_EACH_EXPAND_1(_call, _call_end, ##__VA_ARGS__)
#define _FOR_EACH_EXPAND_3(_call, _call_end, arg, ...) _call(arg) _FOR_EACH_EXPAND_2(_call, _call_end, ##__VA_ARGS__)
#define _FOR_EACH_EXPAND_4(_call, _call_end, arg, ...) _call(arg) _FOR_EACH_EXPAND_3(_call, _call_end, ##__VA_ARGS__)

#define FOR_EACH(_call, _call_end, arg, ...) \
GET_N_ARGS("ignored", ##__VA_ARGS__, _FOR_EACH_EXPAND_4, _FOR_EACH_EXPAND_3, _FOR_EACH_EXPAND_2, _FOR_EACH_EXPAND_1, _FOR_EACH_EXPAND_0)(_call, _call_end, arg, ##__VA_ARGS__)