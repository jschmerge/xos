#ifndef PATH_ITERATOR_H
#define PATH_ITERATOR_H 1

#include <iostream> // XXX remove me

#include <iterator>

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

	path_iterator()
	  : underlying(nullptr)
	  , element_offset(0)
	  , element_value()
		{ }

	path_iterator(const path * p, offset_t off = 0)
	  : underlying(p)
	  , element_offset(off)
	  , element_value()
		{ }

	path_iterator(const path_iterator & other)
	  : underlying(other.underlying)
	  , element_offset(other.element_offset)
	  , element_value(other.element_value)
		{ }

	path_iterator(path_iterator && other)
	  : underlying(std::move(other.underlying))
	  , element_offset(std::move(other.element_offset))
	  , element_value(std::move(other.element_value))
		{ }

	path_iterator & operator = (const path_iterator & other)
	{
		if (this != &other)
		{
			underlying = other.underlying;
			element_offset = other.element_offset;
			element_value = other.element_value;
		}
		return *this;
	}

	path_iterator & operator = (path_iterator && other)
	{
		if (this != &other)
		{
			underlying = std::move(other.underlying);
			element_offset = std::move(other.element_offset);
			element_value = std::move(other.element_value);
		}
		return *this;
	}

	~path_iterator()
	{
		underlying = nullptr;
		element_offset = 0;
	}

	bool operator == (const path_iterator & other)
	{
		return (  (underlying == other.underlying)
		       && (element_offset == other.element_offset) );
	}

	bool operator != (const path_iterator & other)
	{
		return !(*this == other);
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
		//offset_t next = npos;
		const path::string_type & asString = underlying->native();

		if (  (element_offset == 0)
		   && (*(asString.begin()) == path::preferred_separator) )
		{
			std::cout << "-> A <-" << std::endl;
			element_value.clear();
			element_value += path::preferred_separator;

			element_offset =
			  asString.find_first_not_of(path::preferred_separator);

			std::cout << element_value.c_str() << std::endl;
		} else
		{
			std::cout << "-> B <-" << std::endl;
			offset_t new_offset =
			  asString.find_first_of(path::preferred_separator,
			                         element_offset);

			std::cout << element_offset << ' ' << new_offset << ": ";

			if (new_offset != element_offset)
			{

				element_value = asString.substr(element_offset,
				                                new_offset - element_offset);
			} else
			{
				element_value = ".";
				new_offset = npos;
			}

			//while (asString[new_offset] == path::preferred_separator)
			//	++new_offset;

			element_offset = new_offset;

			if (element_offset != npos)
			{
				new_offset = asString.find_first_not_of(
				                 path::preferred_separator, element_offset + 1);

				if (new_offset != npos)
					element_offset = new_offset;
			}

			std::cout << element_value.c_str() << std::endl;
		}

		return *this;
	}

	path_iterator & decrement()
	{
		return *this;
	}

	const path * underlying;
	offset_t element_offset;
	path element_value;
};


} // v1
} // filesystem

#endif // PATH_ITERATOR_H
