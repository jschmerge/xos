#ifndef GUARD_AVL_TREE_H
#define GUARD_AVL_TREE_H 1

#include <memory>
#include <algorithm>

//
// TODO - This class needs noexcept specifications
//
template <typename T,
          typename Compare = std::less<T>,
          typename Allocator = std::allocator<T>>
class avl_tree
{
 public:
	typedef T                  key_type;
	typedef T                  value_type;
	typedef std::size_t        size_type;
	typedef std::ptrdiff_t     difference_type;
	typedef Compare            key_compare;
	typedef Compare            value_compare;
	typedef Allocator          allocator_type;
	typedef value_type       & reference;
	typedef const value_type & const_reference;
	typedef typename std::allocator_traits<Allocator>::pointer
	                           pointer;
	typedef typename std::allocator_traits<Allocator>::const_pointer
	                           const_pointer;
#if 0
	typedef foo                iterator;
	typedef foo                const_iterator;
	typedef std::reverse_iterator<iterator>
	                           reverse_iterator;
	typedef std::reverse_iterator<const_iterator>
	                           const_reverse_iterator;
#endif

	//
	// Constructors
	//
	explicit avl_tree(const Compare & c,
	                  const Allocator & a);
	explicit avl_tree(const Allocator & a);

	template <typename in_iter>
	avl_tree(in_iter begin, in_iter end,
	         const Compare & c = Compare{},
	         const Allocator & a = Allocator{});

	avl_tree(const avl_tree & other);

	avl_tree(const avl_tree & other, const Allocator & a);

	avl_tree(avl_tree && other);

	avl_tree(avl_tree && other, const Allocator & a);

	avl_tree(std::initializer_list<value_type> list,
	         const Compare & c = Compare{},
	         const Allocator & a = Allocator{});

	avl_tree() : avl_tree( Compare{} ) { }

	template <typename in_iter>
	avl_tree(in_iter begin, in_iter end, const Allocator & a)
	  : avl_tree(begin, end, Compare{}, a) { }

	//
	// Destructor
	//
	~avl_tree();

	//
	// Assignment
	//
	avl_tree & operator = (const avl_tree & other);
	avl_tree & operator = (avl_tree && other);
	avl_tree & operator = (std::initializer_list<value_type> list);

	allocator_type get_allocator() const;

	//
	// iterators - not implemented yet
	//
	
	//
	// Capacity
	//
	bool empty() const;

	size_type size() const;

	size_type max_size() const;

	//
	// Modifiers
	//
	void clear();

	// insert() // emplace() // emplace_hint() // erase()

	void swap(avl_tree & other);

	//
	// Lookup
	//
	size_type count(const key_type & key) const;

	template <typename K>
	size_type count(const K & key) const;

	key_compare key_comp() const { return Compare{}; }
	value_compare value_comp() const { return Compare{}; }

	// find()

	
};

#endif // GUARD_AVL_TREE_H

