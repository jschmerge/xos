#ifndef GUARD_PROGRAM_CONFIG_H
#define GUARD_PROGRAM_CONFIG_H 1

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>

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

inline bool has_no_argument(argument_type e)
	{ return ((e & argument_type::arg_mask) == argument_type::none); }

struct state;

typedef std::function<bool(const state &)> state_cb;

typedef std::function<bool(const state &,
                           const state &,
                           const char *)> transit_cb;

//////////////////////////////////////////////////////////////////////
struct config_option
{
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
	{
	}

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

	state(const std::string & n,
	      const config_option * opt,
	      std::function<bool(state&)> on_ingress,
	      std::function<bool(state&)> on_egress)
	  : name(n)
	  , option(opt)
	  , enter(on_ingress)
	  , exit(on_egress)
		{ }

	~state() { }

	state(const state & other) = delete;
	state(state && other) noexcept = delete;
	state & operator = (const state &) = delete;
	state & operator = (state && other) noexcept = delete;

	std::string name;
	const config_option * option;
	std::function<bool(state&)> enter;
	std::function<bool(state&)> exit;

	std::map<int, std::shared_ptr<state>> transitions;
	std::map<int, transit_cb> transition_cb;
};

//////////////////////////////////////////////////////////////////////
class program_config
{
 public:
	virtual ~program_config();

	virtual std::string usage_message(const size_t width = 80) const;

	virtual bool process_option(const config_option & opt,
	                            const std::string & param);

	virtual bool process_option(const config_option & opt);

	virtual void set_program_name(const char * program_path);

	bool parse_command_line(int argc, char ** argv);

	std::vector<std::string> params() const
	{ return nonoption_arguments; }

 protected:
	program_config(const std::initializer_list<config_option> & list);

	void dump_state();

	std::string m_program_name;
	std::vector<config_option> m_options;
	std::vector<std::string> nonoption_arguments;

	bool non_option_start(const char * cp) {
		bool rc = false;
		if (begin_ptr2 == nullptr) {
			begin_ptr2 = cp;
			rc = true;
		}
		return rc;
	}

	bool non_option_end(const char * cp) {
		bool rc = false;
		end_ptr2 = cp;
		if (begin_ptr2) {
			nonoption_arguments.emplace_back(begin_ptr2, end_ptr2);
			printf("####################> NON-OPTION = %s\n",
			       nonoption_arguments.back().c_str());
			begin_ptr2 = end_ptr2 = nullptr;
			rc = true;
		}
		return rc;
	};

	bool parameter_start(const char * cp) {
		bool rc = false;
		if (begin_ptr2 == nullptr) {
			begin_ptr2 = cp;
			rc = true;
		}
		return rc;
	}

	bool parameter_end(const char * cp) {
		bool rc = false;
		end_ptr2 = cp;
		if (begin_ptr2 && current_option) {
			std::string s(begin_ptr2, end_ptr2);
			printf("####################> PARAM = %s\n", s.c_str());
			rc = process_option(*current_option, s);
		}
		current_option = nullptr;
		begin_ptr1 = end_ptr1 = nullptr;
		begin_ptr2 = end_ptr2 = nullptr;
		return rc;
	}

	bool have_short_option(const char * cp) {
		bool rc = false;
		printf("####################> SHORTOPT '%c'\n", *cp);
		begin_ptr1 = end_ptr1 = begin_ptr2 = end_ptr2 = nullptr;
		current_option = nullptr;
		for (const auto & opt : m_options) {
			if (opt.m_short_switch == *cp) {
				current_option = &opt;
				rc = true;
				break;
			}
		}
		if (rc && has_no_argument(current_option->m_argument_type)) {
			rc = process_option(*current_option);
			current_option = nullptr;
		}
		return rc;
	};

 private:

	void declare_state(const std::string & statename,
	                   const config_option * opt = nullptr,
                       state_cb on_ingress = nullptr,
                       state_cb on_egress = nullptr);

	void declare_transition(const std::string & old_state,
                            const std::string & new_state,
                            int value,
	                        transit_cb on_transit = nullptr);

	void declare_option_states();

	void build_parser();
	std::map<std::string, std::shared_ptr<state>> m_states;

	const char * begin_ptr1;
	const char * end_ptr1;
	const char * begin_ptr2;
	const char * end_ptr2;
	const config_option * current_option;

	std::string scratch1;
	std::string scratch2;

};

#endif // GUARD_PROGRAM_CONFIG_H
