#include "path.h"
#include "path_iterator.h"
#include <iostream>


namespace filesystem {
inline namespace v1 {

path_iterator::range_list path_iterator::build_elements(const path * p)
{
	range_list elem;
	const std::string & s = p->generic_string();
	offset_t begin = 0, end = 0;

	if (! p->empty() )
	{
		// deal with any leading path separators
		if (s.at(begin) == path::preferred_separator)
		{
			end = 1;
			elem.push_back(std::make_pair(begin, end));
		}

		do {

			begin = s.find_first_not_of(path::preferred_separator, end);
			end = s.find_first_of(path::preferred_separator, begin);

			if (  (elem.size() != 1) || (! p->has_root_directory())
			   || (begin != npos) || (end != npos) )
			{
				elem.push_back(std::make_pair(begin, end));
			}
		} while (end != npos);
	}

	elem.shrink_to_fit();

	return elem;
}

void path_iterator::create_element_value() const
{
	range r = elements.at(cursor);
	const path::string_type & s = underlying->generic_string();

	if (r.first != npos)
		element_value = ( (r.second != npos) ?
		                  s.substr(r.first, r.second - r.first) :
		                  s.substr(r.first, s.length() - r.first) );
	else
		element_value = ".";
}

path_iterator::path_iterator()
  : underlying(nullptr)
  , elements()
  , cursor(0)
  , element_value()
	{ }

path_iterator::path_iterator(const path * p, state _state)
	  : underlying(p)
	  , elements(build_elements(underlying))
	  , cursor((_state == state::set_to_begin) ?
	           0 : elements.size())
	  , element_value()
		{ }

path_iterator::path_iterator(const path_iterator & other)
  : underlying(other.underlying)
  , elements(other.elements)
  , cursor(other.cursor)
  , element_value(other.element_value)
	{ }

path_iterator::path_iterator(path_iterator && other)
  : underlying(other.underlying)
  , elements(std::move(other.elements))
  , cursor(other.cursor)
  , element_value(std::move(other.element_value))
	{ }

path_iterator::~path_iterator()
	{ }

path_iterator & path_iterator::operator = (const path_iterator & other)
{
	if (this != &other)
	{
		underlying = other.underlying;
		elements = other.elements;
		cursor = other.cursor;
		element_value = other.element_value;
	}
	return *this;
}

path_iterator & path_iterator::operator = (path_iterator && other)
{
	using std::swap;
	if (this != &other)
	{
		swap(underlying, other.underlying);
		swap(elements, other.elements);
		swap(cursor, other.cursor);
		swap(element_value, other.element_value);
	}
	return *this;
}

} // v1
} // filesystem
