#include "config_option.h"

//////////////////////////////////////////////////////////////////////
config_option::config_option(argument_type t, const char * l_opt, char s_opt,
              const char * help)
  : config_option(t, std::string(l_opt != nullptr ? l_opt : ""), s_opt,
                  std::string(help != nullptr ? help : ""))
{ }

//////////////////////////////////////////////////////////////////////
config_option::config_option(argument_type t, const std::string & l_opt, char s_opt,
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

//////////////////////////////////////////////////////////////////////
config_option::config_option(const config_option & other)
  : m_argument_type(other.m_argument_type)
  , m_long_switch(other.m_long_switch)
  , m_short_switch(other.m_short_switch)
  , m_help_message(other.m_help_message)
{ }

//////////////////////////////////////////////////////////////////////
config_option & config_option::operator = (const config_option & other)
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

