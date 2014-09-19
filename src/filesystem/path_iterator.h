#ifndef PATH_ITERATOR_H
#define PATH_ITERATOR_H 1

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
	enum class state
	{
		set_to_begin,
		set_to_end
	};

	typedef path::string_type::size_type offset_t;
	typedef std::pair<offset_t, offset_t> range;
	typedef std::vector<range> range_list;

	static constexpr offset_t npos = std::string::npos;

 public:
	path_iterator();
	path_iterator(const path * p, state _state = state::set_to_begin);
	path_iterator(const path_iterator & other);
	path_iterator(path_iterator && other);

	~path_iterator();

	path_iterator & operator = (const path_iterator & other);
	path_iterator & operator = (path_iterator && other);

	bool operator == (const path_iterator & other)
	{
		return (  (underlying == other.underlying)
		       && (cursor == other.cursor) );
	}

	bool operator != (const path_iterator & other) { return !(*this == other); }

	path_iterator & operator ++ ()
	{
		++cursor;
		return *this;
	}

	path_iterator operator ++ (int)
	{
		path_iterator tmp(*this);
		++(*this);
		return tmp;
	}

	path_iterator & operator -- ()
	{
		--cursor;
		return *this;
	}

	path_iterator operator -- (int)
	{
		path_iterator tmp(*this);
		--(*this);
		return tmp;
	}

	const path & operator * () const
	{
		create_element_value();
		return element_value;
	}

	const path * operator -> () const
	{
		create_element_value();
		return &element_value;
	}

 private:
	void create_element_value() const;
	static range_list build_elements(const path * p);

	const path * underlying;
	range_list elements;
	std::size_t cursor;
	mutable path element_value;
};


} // v1
} // filesystem

#endif // PATH_ITERATOR_H
