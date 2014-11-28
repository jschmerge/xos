#ifndef GUARD_PROGRAM_CONFIG_H
#define GUARD_PROGRAM_CONFIG_H 1

#include <string>
#include <vector>

#include "utility/bitmask_operators.h"

enum class argument_type : uint32_t
{
	none                 = 0x00000000,
	has_argument         = 0x00000001,
	missing_argument_ok  = 0x00000002,
	argument_is_sequence = 0x00000004,
	has_short_switch     = 0x00000008,
	has_long_switch      = 0x00000010,

	optional            = (has_argument | missing_argument_ok),
	required            = (has_argument),
	optional_sequence   = (optional | argument_is_sequence),
	required_sequence   = (required | argument_is_sequence),
};

DEFINE_BITMASK_OPERATORS(argument_type, uint32_t);

struct config_option
{
	argument_type m_argument_type;
	std::string m_long_switch;
	int m_short_switch;
	std::string m_help_message;
};

inline bool operator < (const config_option & a, const config_option & b)
{
	return (a.m_short_switch < b.m_short_switch);
}

class program_config
{
 public:
	virtual ~program_config();

	virtual std::string usage_message(const size_t width = 80);

	virtual bool process_option(const config_option & opt,
	                            const std::string value);

	bool parse_command_line(int argc, char ** argv);

 protected:
	program_config(std::initializer_list<config_option> && list);

	std::string m_program_name;
	std::vector<config_option> m_options;
};

#endif // GUARD_PROGRAM_CONFIG_H
