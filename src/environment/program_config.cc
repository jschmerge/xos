#include <cctype>
#include <cassert>
#include <utility>
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
	}
#if 0
	else
		throw std::logic_error("Option is neither long nor short");
#endif

/*

	if (  m_argument_type == argument_type::required
	   && m_short_switch != 0)
	{
		fmt += "--" + m_long_switch +
		        "/-" + static_cast<char>(m_short_switch) +
		        " <arg>";

	} else if (  m_argument_type == argument_type::required
	          && m_short_switch == 0)
	{
		fmt += "--" + m_long_switch + " <arg>";

	} else if (m_argument_type == argument_type::none)
	{
		fmt += "--" + m_long_switch +
		        "/-" + static_cast<char>(m_short_switch);
	}
*/

	return fmt;
}

//////////////////////////////////////////////////////////////////////
program_config::program_config(
	const std::initializer_list<config_option> & list
)
  : m_program_name("program")
  , m_options(list)
{
}

//////////////////////////////////////////////////////////////////////
program_config::~program_config()
{
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
		do {
			again = false;
			size_t oldLen = line.length();

			std::string fmt = opt.option_synopsis();
			line += '[' + fmt + "] ";
			long_help += "  " + fmt + " : " + opt.m_help_message + '\n';

			if (line.length() >= termWidth)
			{
				line.erase(oldLen);
				usage += line + '\n';

				line = std::string(' ', prefixLen);
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
                                    const std::string)
{
	if (opt.m_long_switch == "help")
		printf("%s", usage_message().c_str());

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
void program_config::declare_state(const std::string & statename)
{
	m_states[statename] = std::make_shared<state>(statename);
}

//////////////////////////////////////////////////////////////////////
void program_config::declare_transition(const std::string & old_state,
                                        const std::string & new_state,
                                        int value)
{
	assert (old_state.c_str() != nullptr);
	assert (new_state.c_str() != nullptr);

	if (isprint(value))
	{
		printf("Declaring transition '%s' -> '%s' on '%c'\n",
		       old_state.c_str(), new_state.c_str(), value);
	} else
	{
		printf("Declaring transition '%s' -> '%s' on '\\x%02x'\n",
		       old_state.c_str(), new_state.c_str(), value);
	}

	fflush(stdout);
	m_states.at(old_state)->transitions[value] = m_states.at(new_state);
}


//////////////////////////////////////////////////////////////////////
void program_config::build_parser()
{
	declare_state("start");
	declare_state("start_of_element");
	declare_state("next_argument");
	declare_state("dash");
	declare_state("dash_dash");
	declare_state("end_of_options");
	declare_state("non_option_arg");
	declare_state("non_option_arg2");
	declare_state("next_as_param");
	declare_state("parameter");

//m_states["start"].transitions[-1] = &m_states["start_of_element"];
	declare_transition("start", "start_of_element", -1);

//m_states["next_argument"].transitions[-1] = &m_states["start_of_element"];
	declare_transition("next_argument", "start_of_element", -1);

//m_states["start_of_element"].transitions['-'] = &m_states["dash"];
	declare_transition("start_of_element", "dash", '-');

//m_states["start_of_element"].transitions[-1] = &m_states["non_option_arg"];
	declare_transition("start_of_element", "non_option_arg", -1);

//m_states["dash"].transitions['-'] = &m_states["dash_dash"];
	declare_transition("dash", "dash_dash", '-');

//m_states["dash"].transitions[0] = &m_states["next_argument"];
	declare_transition("dash", "next_argument", 0);

//m_states["dash_dash"].transitions[0] = &m_states["end_of_options"];
	declare_transition("dash_dash", "end_of_options", 0);

//m_states["next_as_param"].transitions[-1] = &m_states["parameter"];
	declare_transition("next_as_param", "parameter", -1);

//m_states["parameter"].transitions[-1] = &m_states["parameter"];
	declare_transition("parameter", "parameter", -1);

//m_states["parameter"].transitions[0] = &m_states["next_argument"];
	declare_transition("parameter", "next_argument", 0);

//m_states["non_option_arg"].transitions[-1] = &m_states["non_option_arg"];
	declare_transition("non_option_arg", "non_option_arg", -1);

//m_states["non_option_arg"].transitions[0] = &m_states["next_argument"];
	declare_transition("non_option_arg", "next_argument", 0);

//m_states["end_of_options"].transitions[-1] = &m_states["non_option_arg2"];
	declare_transition("end_of_options", "non_option_arg2", -1);

//m_states["non_option_arg2"].transitions[-1] = &m_states["non_option_arg2"];
	declare_transition("non_option_arg2", "non_option_arg2", -1);

//m_states["non_option_arg2"].transitions[0] = &m_states["end_of_options"];
	declare_transition("non_option_arg2", "end_of_options", 0);

	for (const auto & opt : m_options)
	{
		if (opt.m_short_switch != -1)
		{
			std::string shortopt = "-";
			shortopt += opt.m_short_switch;

			declare_state(shortopt);
			declare_transition("dash", shortopt, opt.m_short_switch);

			printf("XXX shortopt %s\n", shortopt.c_str());

			if (is_set(opt.m_argument_type, argument_type::optional))
			{
				declare_transition(shortopt, "parameter", -1);
				declare_transition(shortopt, "next_argument", 0);
			} else if (is_set(opt.m_argument_type, argument_type::required))
			{
				declare_transition(shortopt, "parameter", -1);
				declare_transition(shortopt, "next_as_param", 0);
			} else if((opt.m_argument_type & argument_type::arg_mask) == argument_type::none)
			{
				declare_transition(shortopt, "dash", -1);
				declare_transition(shortopt, "next_argument", 0);
			} else
				assert(0);
		}
 
		if (opt.m_long_switch.size())
		{
			std::string longopt = "--";
			auto last_state = m_states["dash_dash"];

			for (auto c : opt.m_long_switch)
			{
				longopt += c;

				if (m_states.find(longopt) == m_states.end())
					declare_state(longopt);

				declare_transition(last_state->name, longopt, c);

				last_state = m_states[longopt];
			}

			if (is_set(opt.m_argument_type, argument_type::optional))
			{
//				m_states[longopt].transitions[0] = &m_states["next_argument"];
				declare_transition(longopt, "next_argument", 0);
//				m_states[longopt].transitions['='] = &m_states["parameter"];
				declare_transition(longopt, "parameter", '=');
			} else if (is_set(opt.m_argument_type, argument_type::required))
			{
//				m_states[longopt].transitions[0] = &m_states["next_as_param"];
				declare_transition(longopt, "next_as_param", 0);
//				m_states[longopt].transitions['='] = &m_states["parameter"];
				declare_transition(longopt, "parameter", '=');
			} else if ((opt.m_argument_type & argument_type::arg_mask) == argument_type::none)
			{
//				m_states[longopt].transitions[0] = &m_states["next_argument"];
				declare_transition(longopt, "next_argument", 0);
			}
		}
	}

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
//				prev_state->transitions[0] = terminus->transitions[0];
				declare_transition(prev_state->name,
				                   terminus->transitions[0]->name, 0);


				if (terminus->transitions.find('=')
				      != terminus->transitions.end())
				{
//					prev_state->transitions['='] = terminus->transitions['='];
					declare_transition(prev_state->name,
					                   terminus->transitions['=']->name,
					                   '=');
				}

				longopt.pop_back();

				if (longopt != "--")
					prev_state = m_states[longopt];
			}
		}
	}

	dump_state();
}

//////////////////////////////////////////////////////////////////////
bool program_config::parse_command_line(int argc, char ** argv)
{
	set_program_name(argv[0]);
	build_parser();

	auto state_cursor = m_states["start"];

	printf("----------------------------------------------\n");
	printf("PARSING COMMAND LINE:\n");
	for (int i = 1; i < argc; ++i)
	{
		const char * arg = argv[i];
		printf("-----> ARGV[%d] = '%s'\n", i, arg);

		do {
			std::string old_state;
			std::string new_state;

			while (state_cursor->is_placeholder())
			{
				old_state = state_cursor->name;
				state_cursor = state_cursor->transitions.at(-1);
				new_state = state_cursor->name;
				printf("%-28s -> %s\n\n", old_state.c_str(),
			           new_state.c_str());
			}

			old_state = state_cursor->name;

			if (state_cursor->transitions.find(*arg)
			      != state_cursor->transitions.end())
			{
				state_cursor = state_cursor->transitions.at(*arg);
			} else if (state_cursor->transitions.find(-1)
			             != state_cursor->transitions.end())
			{
				state_cursor = state_cursor->transitions.at(-1);
			} else
				throw std::runtime_error("Bad option");

			new_state = state_cursor->name;

			if (std::isprint(*arg))
				printf("(%-20s + '%c') -> %s\n", old_state.c_str(), *arg,
				       new_state.c_str());
			else
				printf("(%-20s + x%02x) -> %s\n", old_state.c_str(), *arg,
				       new_state.c_str());
#if 0
#endif
		} while (*arg++);
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
			if  (y.second  == nullptr)
				throw std::logic_error("I'm dumb");
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

