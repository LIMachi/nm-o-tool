#define IF_ELSE(condition) _IF_ ## condition
#define _IF_1(...) __VA_ARGS__ _IF_1_ELSE
#define _IF_0(...)             _IF_0_ELSE

#define _IF_1_ELSE(...)
#define _IF_0_ELSE(...) __VA_ARGS__

Should be "it was one": IF_ELSE(1)(it was one)(it was zero)

Should be "it was zero": IF_ELSE(0)(it was one)(it was zero)

Oh dear: IF_ELSE(123)(it was one)(it was zero)
