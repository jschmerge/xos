#include <cctype>
#include <cassert>
#include <utility>
#include <functional>
#include <map>
#include <set>
#include <cstring>

#include "program_config.h"

//////////////////////////////////////////////////////////////////////
std::string config_option::option_synopsis() const
{
	std::string fmt;

	if (  is_set(m_argument_type, argument_type::has_short_switch)
	   && is_set(m_argument_type, argument_type::has_long_switch))
	{
		fmt += '-';
		fmt += static_cast<char>(m_short_switch);
		fmt += "/--";
		fmt += m_long_switch;
	} else if (is_set(m_argument_type, argument_type::has_short_switch) )
	{
		fmt += '-';
		fmt += static_cast<char>(m_short_switch);
	} else if (is_set(m_argument_type, argument_type::has_long_switch))
	{
		fmt += "--";
		fmt += m_long_switch;
	} else
		throw std::logic_error("Option is neither long nor short");

	if (is_set(m_argument_type, argument_type::has_argument))
	{
		if (is_set(m_argument_type, argument_type::missing_argument_ok))
			fmt += " [arg]";
		else
			fmt += " <arg>";
	}

	return fmt;
}

//////////////////////////////////////////////////////////////////////
program_config::program_config(
	const std::initializer_list<config_option> & list
)
  : m_program_name("program")
  , m_options(list)
  , nonoption_arguments()
  , begin_ptr1(nullptr)
  , end_ptr1(nullptr)
  , begin_ptr2(nullptr)
  , end_ptr2(nullptr)
  , current_option(nullptr)
{
}

//////////////////////////////////////////////////////////////////////
program_config::~program_config()
{
	destroy_parser();
}

//////////////////////////////////////////////////////////////////////
void program_config::destroy_parser()
{
	for (auto s : m_states)
	{
		s.second->transitions.clear();
	}
}

//////////////////////////////////////////////////////////////////////
std::string program_config::usage_message(const size_t termWidth) const
{
	std::string usage;
	std::string line = "usage: " + m_program_name + ' ';
	std::string long_help;

	size_t prefixLen = line.length();

	for (const auto & opt : m_options)
	{
		bool again = false;
		std::string fmt = opt.option_synopsis();
		long_help += "  " + fmt + " : " + opt.m_help_message + '\n';

		do {
			again = false;
			size_t oldLen = line.length();

			line += '[' + fmt + "] ";

			if (line.length() >= termWidth)
			{
				line.erase(oldLen);
				usage += line + '\n';
				line = std::string(prefixLen, ' ');
				again = true;
			}
		} while (again);
	}

	if (line.length() > prefixLen)
		usage += line + '\n';

	usage += "\nOptions:\n" + long_help;

	return usage;
}

//////////////////////////////////////////////////////////////////////
bool program_config::process_option(const config_option & opt,
                                    const std::string & param)
{
	printf("processing option with param\n");
	printf("Got option '%s' with param '%s'\n",
	       opt.option_synopsis().c_str(), param.c_str());
	return true;
}

//////////////////////////////////////////////////////////////////////
bool program_config::process_option(const config_option & opt)
{
	printf("processing option without param\n");
	if (opt.m_long_switch == "help")
		printf("%s", usage_message().c_str());
	else
		printf("Got option '%s'\n", opt.option_synopsis().c_str());

	return true;
}

//////////////////////////////////////////////////////////////////////
void program_config::set_program_name(const char * program_path)
{
	int idx = strlen(program_path);
	while (program_path[idx] != '/' && idx != 0) { --idx; }
	if (idx != 0) ++idx;
	m_program_name = (program_path + idx);
}

//////////////////////////////////////////////////////////////////////
void
program_config::declare_state(const std::string & statename,
	                          const config_option * opt,
	                          std::function<bool(const state &)> on_ingress,
	                          std::function<bool(const state &)> on_egress)
{
	m_states[statename] =
	  std::make_shared<state>(statename, opt, on_ingress, on_egress);
}

//////////////////////////////////////////////////////////////////////
void program_config::declare_transition(const std::string & old_state,
                                        const std::string & new_state,
                                        int value, transit_cb on_transit)
{
	assert (old_state.c_str() != nullptr);
	assert (new_state.c_str() != nullptr);

#ifndef NDEBUG
	if (isprint(value))
	{
		printf("Declaring transition '%s' -> '%s' on '%c'\n",
		       old_state.c_str(), new_state.c_str(), value);
	} else
	{
		printf("Declaring transition '%s' -> '%s' on '\\x%02x'\n",
		       old_state.c_str(), new_state.c_str(), value);
	}
#endif

	m_states.at(old_state)->transitions[value] = m_states.at(new_state);
	m_states.at(old_state)->transition_cb[value] = on_transit;
}

//////////////////////////////////////////////////////////////////////
void program_config::build_parser()
{
	using namespace std::placeholders;

	destroy_parser();

	transit_cb n_o_start =
	  std::bind(&program_config::non_option_start, this, _3);
	transit_cb n_o_end =
	  std::bind(&program_config::non_option_end, this, _3);
	transit_cb p_start =
	  std::bind(&program_config::parameter_start, this, _3);
	transit_cb p_end =
	  std::bind(&program_config::parameter_end, this, _3);
	transit_cb s_opt =
	  std::bind(&program_config::have_short_option, this, _3);

	transit_cb single_dash =
	  [this] (const state &, const state &, const char *) {
		nonoption_arguments.emplace_back("-");
		return true;
	  };


	transit_cb no_param =
	[this] (const state &, const state &, const char *) {
		bool rc = false;

		if (current_option != nullptr)
		{
				rc = process_option(*current_option);
				current_option = nullptr;
		}

		return rc;
	};

	transit_cb l_start =
	[this] (const state &, const state & to, const char * cp) {
		begin_ptr1 = begin_ptr2 = end_ptr1 = end_ptr2 = nullptr;
		begin_ptr1 = cp;
//		printf("LONG START (rest = %s)\n", cp);
		if (to.option != nullptr) current_option = to.option;
		return true;
	};

	transit_cb l_mid =
	[this] (const state &, const state & to, const char * ) {
		if (to.option != nullptr) current_option = to.option;
		return true;
	};

	transit_cb l_end =
	[this] (const state &, const state &, const char * ) {
		bool rc = false;
		if (current_option)
		{
			rc = process_option(*current_option);
			current_option = nullptr;
		}
		return rc;
	};

	declare_state("start");
	declare_state("dash");
	declare_state("dash_dash");
	declare_state("non_option_arg");
	declare_state("non_option_arg2");
	declare_state("parameter");
	declare_state("short_no_param");
	declare_state("short_opt_param");
	declare_state("short_req_param");

	declare_transition("start", "dash", '-');
	declare_transition("start", "non_option_arg", -1, n_o_start);
	declare_transition("dash", "dash_dash", '-');
	declare_transition("dash", "start", 0, single_dash);
	declare_transition("parameter", "parameter", -1, p_start);
	declare_transition("parameter", "start", 0, p_end);
	declare_transition("non_option_arg", "non_option_arg", -1);
	declare_transition("non_option_arg", "start", 0, n_o_end);
	declare_transition("dash_dash", "non_option_arg2", 0);
	declare_transition("non_option_arg2", "non_option_arg2", -1, n_o_start);
	declare_transition("non_option_arg2", "non_option_arg2", 0, n_o_end);

	declare_transition("short_no_param", "start", 0);
	declare_transition("short_opt_param", "parameter", -1, p_start);
	declare_transition("short_opt_param", "start", 0, no_param);
	declare_transition("short_req_param", "parameter", -1, p_start);
	declare_transition("short_req_param", "parameter", 0);

	for (const auto & opt : m_options)
	{
		// Handle short option
		if (is_set(opt.m_argument_type, argument_type::has_short_switch))
		{
			std::string shortopt = "-";
			shortopt += opt.m_short_switch;

			if (is_set(opt.m_argument_type, argument_type::optional))
			{
				declare_transition("dash", "short_opt_param",
				                   opt.m_short_switch, s_opt);
				declare_transition("short_no_param", "short_opt_param",
				                   opt.m_short_switch, s_opt);

			} else if (is_set(opt.m_argument_type, argument_type::required))
			{
				declare_transition("dash", "short_req_param",
				                   opt.m_short_switch, s_opt);
				declare_transition("short_no_param", "short_req_param",
				                   opt.m_short_switch, s_opt);

			} else if (has_no_argument(opt.m_argument_type))
			{
				declare_transition("dash", "short_no_param",
				                   opt.m_short_switch, s_opt);
				declare_transition("short_no_param", "short_no_param",
				                   opt.m_short_switch, s_opt);

			} else
				throw std::logic_error(
				        "option does not have paramters set correctly");
		}
 
		// Handle long option
		if (is_set(opt.m_argument_type, argument_type::has_long_switch))
		{
			std::string longopt = "--";
			auto last_state = m_states["dash_dash"];

//			for (auto c : opt.m_long_switch)
			for (std::string::size_type i = 0;
			     i < opt.m_long_switch.size();
			     ++i)
			{
				longopt += opt.m_long_switch[i];

				
				auto tmp = m_states.find(longopt);
				if (tmp != m_states.end())
				{
					tmp->second->option = nullptr;
				} else
					declare_state(longopt, &opt);

				if (i == 0)
				{
					declare_transition(last_state->name, longopt,
					                   opt.m_long_switch[i], l_start);
				} else
				{
					declare_transition(last_state->name, longopt,
					                   opt.m_long_switch[i], l_mid);
				}

				last_state = m_states[longopt];
			}

			last_state->option = &opt;

			if (is_set(opt.m_argument_type, argument_type::optional))
			{
				declare_transition(longopt, "start", 0, l_end);
				declare_transition(longopt, "parameter", '=');

			} else if (is_set(opt.m_argument_type, argument_type::required))
			{
				declare_transition(longopt, "parameter", 0);
				declare_transition(longopt, "parameter", '=');

			} else if (has_no_argument(opt.m_argument_type))
			{
				declare_transition(longopt, "start", 0, l_end);

			} else
				throw std::logic_error(
				        "option does not have paramters set correctly");
		}
	}

	// Patch up long option
	for (const auto & opt : m_options)
	{
		if (opt.m_long_switch.size())
		{
			std::string longopt = "--" + opt.m_long_switch;

			auto terminus = m_states[longopt];

			assert(terminus != nullptr);

			longopt.pop_back();
			auto prev_state = m_states[longopt];

			assert(prev_state != nullptr);

			while (longopt != "--" && prev_state->transitions.size() == 1)
			{
				if (is_set(opt.m_argument_type, argument_type::optional))
				{
					declare_transition(prev_state->name,
					                   terminus->transitions[0]->name,
					                   0, l_end);
				} else if (is_set(opt.m_argument_type, argument_type::required))
				{
					declare_transition(prev_state->name,
					                   terminus->transitions[0]->name, 0);
				} else
				{
					declare_transition(prev_state->name,
					                   terminus->transitions[0]->name,
					                   0, l_end);
				}

				if (terminus->transitions.find('=')
				      != terminus->transitions.end())
				{
					declare_transition(prev_state->name,
					                   terminus->transitions['=']->name, '=');
				}

				longopt.pop_back();

				if (longopt != "--")
					prev_state = m_states[longopt];
			}
		}
	}

#ifndef NDEBUG
	dump_state();
#endif
}

//////////////////////////////////////////////////////////////////////
bool program_config::parse_command_line(int argc,
                                        const char * const * const argv)
{
	set_program_name(argv[0]);
	build_parser();

	auto state_cursor = m_states["start"];

#ifndef NDEBUG
	printf("------------------------------------------------------------\n");
	printf("PARSING COMMAND LINE:\n");
#endif
	for (int i = 1; i < argc; ++i)
	{
		const char * arg = argv[i];
#ifndef NDEBUG
		printf("-----> ARGV[%d] = '%s'\n", i, arg);
#endif

		do {
			std::string old_state;
			std::string new_state;

			old_state = state_cursor->name;

			if (state_cursor->transitions.find(*arg)
			      != state_cursor->transitions.end())
			{
				if (state_cursor->exit) state_cursor->exit(*state_cursor);

				if (state_cursor->transition_cb[*arg])
					state_cursor->transition_cb[*arg](
						*state_cursor,
						*(state_cursor->transitions.at(*arg)),
						arg);

				state_cursor = state_cursor->transitions.at(*arg);
				if (state_cursor->exit) state_cursor->enter(*state_cursor);

			} else if (state_cursor->transitions.find(-1)
			             != state_cursor->transitions.end())
			{
				if (state_cursor->exit) state_cursor->exit(*state_cursor);

				if (state_cursor->transition_cb[-1])
					state_cursor->transition_cb[-1](
						*state_cursor,
						*(state_cursor->transitions.at(-1)),
						arg);

				state_cursor = state_cursor->transitions.at(-1);
				if (state_cursor->enter) state_cursor->enter(*state_cursor);

			} else
			{
				if (state_cursor->name == "dash_dash")
					throw std::runtime_error(
					        std::string("Bad option - ") + arg);
				else
					throw std::runtime_error(
					        std::string("Bad option - ") + *arg);
			}

			new_state = state_cursor->name;

#ifndef NDEBUG
			if (std::isprint(*arg))
			{
				printf("\t(%-20s + '%c') -> %s\n", old_state.c_str(), *arg,
				       new_state.c_str());
				if (state_cursor->option != nullptr)
				{
					printf("\t\t-> looking at option %s\n",
					       state_cursor->option->m_long_switch.c_str());
				}
			} else
			{
				printf("\t(%-20s + x%02x) -> %s\n", old_state.c_str(), *arg,
				       new_state.c_str());
				if (state_cursor->option != nullptr)
				{
					printf("\t\t-> looking at option %s\n",
					       state_cursor->option->m_long_switch.c_str());
				}
			}
#endif
		} while (*arg++);
	}

	if ( ! ( (state_cursor == m_states["start"])
	       || (state_cursor == m_states["non_option_arg2"]) ) )
	{
		throw std::runtime_error(std::string("Option '")
		                        + current_option->option_synopsis()
		                        + "' requires arument");
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
void program_config::dump_state()
{
	for (auto & x : m_states)
	{
		printf("STATE: %s\n", x.first.c_str());
		for (const auto & y : x.second->transitions)
		{
			if (y.first > 0)
				printf("\t'%c' -> %s\n", y.first,
				       y.second->name.c_str());
			else if (y.first == 0)
				printf("\tNUL -> %s\n", y.second->name.c_str());
			else
				printf("\tDEFAULT -> %s\n", y.second->name.c_str());
		}
	}

	printf("-> Total of %zd states defined\n", m_states.size());
}

