#include <cstdio>

#include "program_config.h"

class my_config : public program_config
{
 public:
	my_config()
	  : program_config({
		{ argument_type::none, "help", 'h', "Prints this message" },
		{ argument_type::required, "required", 'r', "Required argument" },
		{ argument_type::optional, "optional", 'o', "Optional argument" },
		{ argument_type::none, "longfakeout",'l', "Long opt without param" },
		{ argument_type::optional, "long-only", -1, "No short option" },
	    })
	{
	}
};

int main(int argc, char ** argv)
{
	my_config conf;

	conf.parse_command_line(argc, argv);
	printf("------------------------------------\n"
	       "%s", conf.usage_message(80).c_str());
	return 0;
}
