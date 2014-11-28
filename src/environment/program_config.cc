#include "program_config.h"

#if 0
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
	argument_type arg_type;
	std::string long_switch;
	int short_switch;
	std::string help_message;
};

inline bool operator < (const config_option & a, const config_option & b)
{
	return (a.short_switch < b.short_switch);
}

 protected:

	std::set<config_option> m_options;
#endif

program_config::program_config(std::initializer_list<config_option> && list)
  : m_program_name("program")
  , m_options(list)
{
}

program_config::~program_config()
{
}


std::string program_config::usage_message(const size_t termWidth)
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


			if (  opt.m_argument_type == argument_type::required
			   && opt.m_short_switch != 0)
			{
				line += "[--" + opt.m_long_switch +
				        "|-" + static_cast<char>(opt.m_short_switch) +
				        " <arg>] ";

				long_help += "[--" + opt.m_long_switch +
				             "|-" + static_cast<char>(opt.m_short_switch) +
				             " <arg>] : " + opt.m_help_message + '\n';

			} else if (  opt.m_argument_type == argument_type::required
			          && opt.m_short_switch == 0)
			{
				line += "[--" + opt.m_long_switch + " <arg>] ";

				long_help += "[--" + opt.m_long_switch + " <arg>] : "
				             + opt.m_help_message + '\n';

			} else if (opt.m_argument_type == argument_type::none)
			{
				line += "[--" + opt.m_long_switch +
				        "|-" + static_cast<char>(opt.m_short_switch) +
				        "] ";
				long_help += "[--" + opt.m_long_switch +
				             "|-" + static_cast<char>(opt.m_short_switch) +
				             "] : " + opt.m_help_message + '\n';
			}

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

	usage += long_help;

	return usage;
}

bool program_config::process_option(const config_option & opt,
                                    const std::string)
{
	if (opt.m_long_switch == "help")
		printf("%s", usage_message().c_str());

	return true;
}

#if 0

bool parse_command_line(int argc, char ** argv);
#endif

