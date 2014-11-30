#ifndef GUARD_PROGRAM_CONFIG_H
#define GUARD_PROGRAM_CONFIG_H 1

#include <string>
#include <vector>
#include <map>

#include "utility/bitmask_operators.h"

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

DEFINE_BITMASK_OPERATORS(argument_type, uint32_t);

//////////////////////////////////////////////////////////////////////
struct config_option
{
#if 0
	config_option(argument_type t, const std::string & l_opt, char s_opt,
	              const std::string & help)
	  : m_argument_type(t & argument_type::arg_mask)
	  , m_long_switch(l_opt)
	  , m_short_switch(s_opt)
	  , m_help_message(help)
	{
		if (m_short_switch != -1)
			m_argument_type |= argument_type::has_short_switch;
		else
			m_argument_type &= ~argument_type::has_short_switch;

		if (m_long_switch.size())
			m_argument_type |= argument_type::has_long_switch;
		else
			m_argument_type &= ~argument_type::has_long_switch;
	}

	config_option(const config_option & other)
	  : m_argument_type(other.m_argument_type)
	  , m_long_switch(other.m_long_switch)
	  , m_short_switch(other.m_short_switch)
	  , m_help_message(other.m_help_message)
		{ }

	config_option & operator = (const config_option & other)
	{
		if (this != &other)
		{
			m_argument_type = other.m_argument_type;
			m_long_switch = other.m_long_switch;
			m_short_switch = other.m_short_switch;
			m_help_message = other.m_help_message;
		}
		return *this;
	}
#endif

	argument_type m_argument_type;
	std::string m_long_switch;
	int m_short_switch;
	std::string m_help_message;

	std::string option_synopsis() const;
};

//////////////////////////////////////////////////////////////////////
struct state
{
	state() : name() { }

	state(const std::string & n)
	  : name(n)
		{ }

	state(const state & other) = delete;

	state(state && other) noexcept
	  : name(std::move(other.name))
	  , transitions(std::move(other.transitions))
		{ }

	state & operator = (const state &) = delete;

	state & operator = (state && other) noexcept
	{
		using std::swap;
		swap(name, other.name);
		swap(transitions, other.transitions);
		return *this;
	}

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

	void dump_state();

	std::string m_program_name;
	std::vector<config_option> m_options;

 private:
	void build_parser();
	std::map<std::string, state> m_states;

};

#endif // GUARD_PROGRAM_CONFIG_H
