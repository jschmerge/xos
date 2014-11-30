#include <utility>
#include <cctype>
#include <map>
#include <set>
#include <cstring>

#include "program_config.h"

//////////////////////////////////////////////////////////////////////
std::string config_option::option_synopsis() const
{
	std::string fmt;
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

	return fmt;
}

//////////////////////////////////////////////////////////////////////
program_config::program_config(std::initializer_list<config_option> && list)
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
void program_config::build_parser()
{

	m_states["start"] = state{ "start" };
	m_states["start_of_element"] = state{ "start_of_element" };
	m_states["next_argument"] = state{ "next_argument" };
	m_states["dash"] = state("dash");
	m_states["dash_dash"] = state("dash_dash");
	m_states["end_of_options"] = state("end_of_options");
	m_states["non_option_arg"] = state("non_option_arg");
	m_states["non_option_arg2"] = state("non_option_arg2");
	m_states["next_as_param"] = state("next_as_param");
	m_states["parameter"] = state("parameter");
	
	m_states["start"].transitions[-1] = &m_states["start_of_element"];

	m_states["start_of_element"].transitions['-'] = &m_states["dash"];
	m_states["start_of_element"].transitions[-1] = &m_states["non_option_arg"];

	m_states["next_argument"].transitions[-1] = &m_states["start_of_element"];

	m_states["dash"].transitions['-'] = &m_states["dash_dash"];
	m_states["dash"].transitions[0] = &m_states["next_argument"];

	m_states["dash_dash"].transitions[0] = &m_states["end_of_options"];

	m_states["next_as_param"].transitions[-1] = &m_states["parameter"];

	m_states["parameter"].transitions[-1] = &m_states["parameter"];
	m_states["parameter"].transitions[0] = &m_states["next_argument"];

	m_states["non_option_arg"].transitions[-1] = &m_states["non_option_arg"];
	m_states["non_option_arg"].transitions[0] = &m_states["next_argument"];

	m_states["end_of_options"].transitions[-1] = &m_states["non_option_arg2"];

	m_states["non_option_arg2"].transitions[-1] = &m_states["non_option_arg2"];
	m_states["non_option_arg2"].transitions[0] = &m_states["end_of_options"];

	for (const auto & opt : m_options)
	{
		if (opt.m_short_switch != -1)
		{
			std::string shortopt = "-";
			shortopt += opt.m_short_switch;

			m_states[shortopt] = state(shortopt);
			m_states["dash"].transitions[opt.m_short_switch] =
		                                                  &m_states[shortopt];
			if (is_set(opt.m_argument_type, argument_type::optional))
			{
				m_states[shortopt].transitions[-1] = &m_states["parameter"];
				m_states[shortopt].transitions[0] = &m_states["next_argument"];
			} else if (is_set(opt.m_argument_type, argument_type::required))
			{
				m_states[shortopt].transitions[-1] = &m_states["parameter"];
				m_states[shortopt].transitions[0] = &m_states["next_as_param"];
			} else if (opt.m_argument_type == argument_type::none)
			{
				m_states[shortopt].transitions[-1] = &m_states["dash"];
				m_states[shortopt].transitions[0] = &m_states["next_argument"];
			}
		}
 
		if (opt.m_long_switch.size())
		{
			std::string longopt = "--";
			state * last_state = &m_states["dash_dash"];

			for (auto c : opt.m_long_switch)
			{
				longopt += c;

				if (m_states.find(longopt) == m_states.end())
					m_states[longopt] = state(longopt);

				last_state = last_state->transitions[c] = &m_states[longopt];
			}

			if (is_set(opt.m_argument_type, argument_type::optional))
			{
				m_states[longopt].transitions[0] = &m_states["next_argument"];
				m_states[longopt].transitions['='] = &m_states["parameter"];
			} else if (is_set(opt.m_argument_type, argument_type::required))
			{
				m_states[longopt].transitions[0] = &m_states["next_as_param"];
				m_states[longopt].transitions['='] = &m_states["parameter"];
			} else if (opt.m_argument_type == argument_type::none)
			{
				m_states[longopt].transitions[0] = &m_states["next_argument"];
			}
		}
	}

	for (const auto & opt : m_options)
	{
		if (opt.m_long_switch.size())
		{
			std::string longopt = "--" + opt.m_long_switch;

			state * terminus = &m_states[longopt];

			longopt.pop_back();
			state * prev_state = &m_states[longopt];

			while (prev_state->transitions.size() == 1)
			{
				printf("###==> %s is unique\n", longopt.c_str());

				prev_state->transitions[0] = terminus->transitions[0];
				if (terminus->transitions.find('=')
				      != terminus->transitions.end())
				{
					prev_state->transitions['='] = terminus->transitions['='];
				}

				longopt.pop_back();
				prev_state = &m_states[longopt];
			}
		}
	}

	for (auto & x : m_states)
	{
		printf("STATE: %s\n", x.first.c_str());
		for (const std::pair<int, state*> & y : x.second.transitions)
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

	printf("-> Total of %zd states defied\n", m_states.size());
}

//////////////////////////////////////////////////////////////////////
bool program_config::parse_command_line(int argc, char ** argv)
{
	set_program_name(argv[0]);
	build_parser();

	const state * state_cursor = &m_states["start"];

	printf("----------------------------------------------\n");
	for (int i = 1; i < argc; ++i)
	{
		const char * arg = argv[i];

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

//			state_cursor = state_cursor->transitions[*arg];
			new_state = state_cursor->name;

			if (std::isprint(*arg))
				printf("(%-20s + '%c') -> %s\n", old_state.c_str(), *arg,
				       new_state.c_str());
			else
				printf("(%-20s + x%02x) -> %s\n", old_state.c_str(), *arg,
				       new_state.c_str());
		} while (*arg++);
	}

	return true;
}
