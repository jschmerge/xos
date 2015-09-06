#ifndef GUARD_PROGRAM_CONFIG_H
#define GUARD_PROGRAM_CONFIG_H 1

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>

#include "config_option.h"
//#include "utility/bitmask_operators.h"

//////////////////////////////////////////////////////////////////////
struct state;

typedef std::function<bool(const state &,
                           const state &,
                           const char *)> transit_cb;

//////////////////////////////////////////////////////////////////////
struct state
{
	state() : name() { }

	state(const std::string & n,
	      const config_option * opt)
	  : name(n)
	  , option(opt)
		{ }

	~state() { }

	state(const state & other) = delete;
	state(state && other) noexcept = delete;
	state & operator = (const state &) = delete;
	state & operator = (state && other) noexcept = delete;

	std::string name;
	const config_option * option;

	std::map<int, state*> transitions;
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

	bool parse_command_line(int argc, const char * const * const argv);

	std::vector<std::string> & params()
		{ return nonoption_arguments; }

	const std::vector<std::string> & params() const
		{ return nonoption_arguments; }

 protected:
	program_config(const std::initializer_list<config_option> & list);

	void dump_state();

	std::string m_program_name;
	std::vector<config_option> m_options;
	std::vector<std::string> nonoption_arguments;

	std::map<std::string, std::shared_ptr<state>> m_states;

	const char * begin_ptr1;
	const char * begin_ptr2;
	const config_option * current_option;

 private:
	void destroy_parser();
	bool non_option_start(const char * cp);
	bool non_option_end(const char * cp);
	bool parameter_start(const char * cp);
	bool parameter_end(const char * cp);
	bool have_short_option(const char * cp);

	void declare_state(const std::string & statename,
	                   const config_option * opt = nullptr);

	void declare_transition(const std::string & old_state,
                            const std::string & new_state,
                            int value,
	                        transit_cb on_transit = nullptr);

	void declare_option_states();

	void build_parser();
};

#endif // GUARD_PROGRAM_CONFIG_H
