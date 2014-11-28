#ifndef GUARD_PROGRAM_CONFIG_H
#define GUARD_PROGRAM_CONFIG_H 1

#include <string>
#include <set>

enum class argument_type
{
	none,
	optional,
	required,
	optional_list,
	required_list,
};

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
	std::set<config_option> m_options;
};

#endif // GUARD_PROGRAM_CONFIG_H
