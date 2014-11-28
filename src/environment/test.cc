#include <cstdio>

#include "program_config.h"

class my_config : public program_config
{
 public:
	my_config()
	  : program_config({
		{ argument_type::none, "help", 'h', "Prints this message" },
		{ argument_type::required, "required", 'r', "Required argument" },
		{ argument_type::required, "optional", 'o', "Optional argument" },
	    })
	{
	}
};

int main(int, char **)
{
	my_config conf;

	printf("this is a test\n%s", conf.usage_message(80).c_str());
	return 0;
}
