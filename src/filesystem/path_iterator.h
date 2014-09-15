#ifndef PATH_ITERATOR_H
#define PATH_ITERATOR_H 1

#include <iterator>

namespace filesystem {
inline namespace v1 {

class path;

class path_iterator
  : public std::iterator<std::bidirectional_iterator_tag, path, void>
{
 public:
	path_iterator();
	path_iterator(const path_iterator &);
	path_iterator(path_iterator &&);
	path_iterator & operator = (const path_iterator &);
	path_iterator & operator = (path_iterator &&);
	~path_iterator();

	path_iterator & operator ++ ();
	path_iterator & operator ++ (int);
	path_iterator & operator -- ();
	path_iterator & operator -- (int);

	const path & operator * () const;
	const path & operator -> () const;
};


} // v1
} // filesystem

#endif // PATH_ITERATOR_H
