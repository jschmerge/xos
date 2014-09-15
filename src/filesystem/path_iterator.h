#ifndef PATH_ITERATOR_H
#define PATH_ITERATOR_H 1

#include <iostream> // XXX

#include <iterator>
#include <utility>
#include <vector>

#include "path.h"

namespace filesystem {
inline namespace v1 {

class path;

class path_iterator
  : public std::iterator<std::bidirectional_iterator_tag, path, void>
{
 public:
	typedef path::string_type::size_type offset_t;
	static constexpr offset_t npos = std::string::npos;

 private:
	static
	std::vector<std::pair<offset_t, offset_t>> build_elements(const path * p)
	{
		std::vector<std::pair<offset_t, offset_t>> elem;
		const std::string & s = p->generic_string();
		offset_t begin = 0, end = 0;

		if (! p->empty() )
		{
			// deal with any leading path separators
			if (s.at(begin) == path::preferred_separator)
			{
				end = 1;
				elem.push_back(std::make_pair(begin, end));

//				begin = s.find_first_not_of(path::preferred_separator, end);
//				end = s.find_first_of(path::preferred_separator, begin);
			}

			do {

				begin = s.find_first_not_of(path::preferred_separator, end);
				end = s.find_first_of(path::preferred_separator, begin);

				elem.push_back(std::make_pair(begin, end));
			}
			while (end != npos);

			std::cout << "---> (" << begin << ", " << end << ")\n";
		}

		for (auto & x : elem)
		{
			std::cout << '(' << x.first << ", " << x.second << "): ";
			if (x.first == npos && x.second == npos)
				std::cout << "\'.\'" << std::endl;
			else if (x.second == npos)
				std::cout << s.substr(x.first) << std::endl;
			else
				std::cout << s.substr(x.first, x.second) << std::endl;
		}

		return elem;
	}

 public:
	path_iterator()
	  : underlying(nullptr)
	  , element_value()
		{ }

	path_iterator(const path * p)
	  : underlying(p)
	  , elements(build_elements(underlying))
	  , element_value()
		{ }

	path_iterator(const path_iterator & other)
	  : underlying(other.underlying)
	  , element_value(other.element_value)
		{ }

	path_iterator(path_iterator && other)
	  : underlying(std::move(other.underlying))
	  , element_value(std::move(other.element_value))
		{ }

	path_iterator & operator = (const path_iterator & other)
	{
		if (this != &other)
		{
			underlying = other.underlying;
			element_value = other.element_value;
		}
		return *this;
	}

	path_iterator & operator = (path_iterator && other)
	{
		if (this != &other)
		{
			underlying = std::move(other.underlying);
			element_value = std::move(other.element_value);
		}
		return *this;
	}

	~path_iterator()
	{
		underlying = nullptr;
	}

	bool operator == (const path_iterator & )
	{
		return false;
	}

	bool operator != (const path_iterator & )
	{
		return true;
	}

	path_iterator & operator ++ ()
		{ return increment(); }

	path_iterator & operator ++ (int);

	path_iterator & operator -- ();
	path_iterator & operator -- (int);

	const path & operator * () const;
	const path & operator -> () const;

 private:
	path_iterator & increment()
	{
		return *this;
	}

	path_iterator & decrement()
	{
		return *this;
	}

	const path * underlying;
	std::vector<std::pair<offset_t, offset_t>> elements;
	path element_value;
};


} // v1
} // filesystem

#endif // PATH_ITERATOR_H
