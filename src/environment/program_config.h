#ifndef GUARD_PROGRAM_CONFIG_H
#define GUARD_PROGRAM_CONFIG_H 1

#include <string>
#include <vector>
#include <map>

#include "utility/bitmask_operators.h"

//////////////////////////////////////////////////////////////////////
enum class argument_type : uint8_t
{
	none                 = 0x00000000,
	has_argument         = 0x00000001,
	missing_argument_ok  = 0x00000002,
//	argument_is_sequence = 0x00000004,
	                       // 8 unused
	has_short_switch     = 0x00000010,
	has_long_switch      = 0x00000020,

	optional             = (has_argument | missing_argument_ok),
	required             = (has_argument),
//	optional_sequence    = (optional | argument_is_sequence),
//	required_sequence    = (required | argument_is_sequence),
};

DEFINE_BITMASK_OPERATORS(argument_type, uint8_t);

//////////////////////////////////////////////////////////////////////
struct config_option
{
	argument_type m_argument_type;
	std::string m_long_switch;
	int m_short_switch;
	std::string m_help_message;

	std::string option_synopsis() const;
};

struct state
{
	state() : name() { }
	state(const std::string & n) : name(n) { }

	std::string name;
	std::map<int, state*> transitions;

	bool is_placeholder() const
	{ return (  transitions.size() == 1
	         && transitions.find(-1) != transitions.end()); }
};


//////////////////////////////////////////////////////////////////////
class program_config
{
 public:
	virtual ~program_config();

	virtual std::string usage_message(const size_t width = 80) const;

	virtual bool process_option(const config_option & opt,
	                            const std::string value);

	virtual void set_program_name(const char * program_path);

	bool parse_command_line(int argc, char ** argv);

 protected:
	program_config(std::initializer_list<config_option> && list);

	std::string m_program_name;
	std::vector<config_option> m_options;

 private:
	void build_parser();
	std::map<std::string, state> m_states;
};

#endif // GUARD_PROGRAM_CONFIG_H
