#ifndef PATH_ITERATOR_H
#define PATH_ITERATOR_H 1

#include <iterator>

#include <path.h>

namespace filesystem {
inline namespace v1 {

class path;

class path_iterator
  : public std::iterator<std::bidirectional_iterator_tag, path, void>
{
 public:
	path_iterator()
	  : underlying(nullptr), element_offset(0) { }

	path_iterator(const path_iterator & other)
	  : underlying(other.underlying), element_offset(other.element_offset) { }

	path_iterator(path_iterator && other)
	  : underlying(std::move(other.underlying))
	  , element_offset(std::move(other.element_offset)) { }

	path_iterator & operator = (const path_iterator & other)
	{
		if (this != &other)
		{
			underlying = other.underlying;
			element_offset = other.element_offset;
		}
		return *this;
	}

	path_iterator & operator = (path_iterator && other)
	{
		if (this != &other)
		{
			underlying = std::move(other.underlying);
			element_offset = std::move(other.element_offset);
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

	path_iterator & operator ++ ();
	path_iterator & operator ++ (int);

	path_iterator & operator -- ();
	path_iterator & operator -- (int);

	const path & operator * () const;
	const path & operator -> () const;

 private:
	typedef path::string_type::size_type offset_t;

	path * underlying;
	offset_t element_offset;
};


} // v1
} // filesystem

#endif // PATH_ITERATOR_H
