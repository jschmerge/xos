#ifndef GUARD_BITMASK_OPERATORS_H
#define GUARD_BITMASK_OPERATORS_H 1

// Macro for defining bitwise operators for enum types
#define DEFINE_BITMASK_OPERATORS(enum_type, int_type)                \
	inline constexpr enum_type operator & (enum_type a, enum_type b) \
	{ return static_cast<enum_type>(                                 \
		static_cast<int_type>(a) & static_cast<int_type>(b)); }      \
	inline constexpr enum_type operator | (enum_type a, enum_type b) \
	{ return static_cast<enum_type>(                                 \
		static_cast<int_type>(a) | static_cast<int_type>(b)); }      \
	inline constexpr enum_type operator ^ (enum_type a, enum_type b) \
	{ return static_cast<enum_type>(                                 \
		static_cast<int_type>(a) ^ static_cast<int_type>(b)); }      \
	inline constexpr enum_type operator ~ (enum_type a)              \
	{ return static_cast<enum_type>(~static_cast<int_type>(a)); }    \
	inline enum_type & operator &= (enum_type & a, enum_type & b)    \
	{ a = (a & b); return a; }                                       \
	inline enum_type & operator |= (enum_type & a, enum_type & b)    \
	{ a = (a | b); return a; }                                       \
	inline enum_type & operator ^= (enum_type & a, enum_type & b)    \
	{ a = (a ^ b); return a; }


#endif // GUARD_BITMASK_OPERATORS_H
