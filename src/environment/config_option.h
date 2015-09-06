#ifndef GUARD_CONFIG_OPTION_H
#define GUARD_CONFIG_OPTION_H 1

#include <cstdint>
#include <functional>
#include <string>

//////////////////////////////////////////////////////////////////////
enum class argument_type : uint32_t
{
	none                 = 0x00000000,
	has_argument         = 0x00000001,
	missing_argument_ok  = 0x00000002,
	                       // 4 unused
	                       // 8 unused
	arg_mask             = 0x0000000f,
	has_short_switch     = 0x00000010,
	has_long_switch      = 0x00000020,

	optional             = (has_argument | missing_argument_ok),
	required             = (has_argument),
};

inline constexpr argument_type operator & (argument_type a, argument_type b)
	{ return static_cast<argument_type>( static_cast<uint32_t>(a)
	                                   & static_cast<uint32_t>(b) ); }

inline constexpr argument_type operator | (argument_type a, argument_type b)
	{ return static_cast<argument_type>( static_cast<uint32_t>(a)
	                                   | static_cast<uint32_t>(b) ); }

inline constexpr argument_type operator ^ (argument_type a, argument_type b)
	{ return static_cast<argument_type>( static_cast<uint32_t>(a)
	                                   ^ static_cast<uint32_t>(b)); }

inline constexpr argument_type operator ~ (argument_type a)
	{ return static_cast<argument_type>(~static_cast<uint32_t>(a)); }

inline argument_type & operator &= (argument_type & a, const argument_type & b)
	{ a = (a & b); return a; }

inline argument_type & operator |= (argument_type & a, const argument_type & b)
	{ a = (a | b); return a; }

inline argument_type & operator ^= (argument_type & a, const argument_type & b)
	{ a = (a ^ b); return a; }

inline constexpr bool is_set(argument_type set, argument_type value)
	{ return ((set & value) == value); } 

inline bool has_no_argument(argument_type e)
	{ return ((e & argument_type::arg_mask) == argument_type::none); }

//////////////////////////////////////////////////////////////////////
struct config_option
{
	typedef std::function<bool(const config_option &)> pressence_cb;

	typedef std::function<bool(const config_option &,
	                           const std::string &)> argument_cb;

	config_option(argument_type t, const char * l_opt, char s_opt,
	              const char * help);

	config_option(argument_type t, const std::string & l_opt, char s_opt,
	              const std::string & help);

	config_option(const config_option & other);

	config_option & operator = (const config_option & other);

	argument_type m_argument_type;
	std::string m_long_switch;
	int m_short_switch;
	std::string m_help_message;

	std::string option_synopsis() const;
};

#endif // GUARD_CONFIG_OPTION_H
