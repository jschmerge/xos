#ifndef GUARD_AVL_TREE_H
#define GUARD_AVL_TREE_H 1

#include <memory>
#include <algorithm>
#include <iterator>

#include <cstdio>
#include <cassert>

// forward declaration
template <typename T, typename C, typename A> class avl_tree;

template <typename T>
struct avl_tree_node
{
	typedef T value_type;

	value_type value;
	avl_tree_node * parent, * left, * right;

	int8_t balance;

	avl_tree_node(const value_type & _value)
	  : value(_value)
	  , parent(nullptr)
	  , left(nullptr)
	  , right(nullptr)
	  , balance(0)
		{ }

	avl_tree_node(value_type && _value)
	  : value(std::move<T>(_value))
	  , parent(nullptr)
	  , left(nullptr)
	  , right(nullptr)
	  , balance(0)
		{ }

	~avl_tree_node()
		{ assert(left == nullptr && right == nullptr); }
};

enum class avl_iterator_orientation
{
	none,
	right,
	left,
	up,
	end
};

template <typename T, typename C, typename A>
class avl_tree_iterator
  : public std::iterator<std::bidirectional_iterator_tag, T>
{
 public:
	avl_tree_iterator();
	avl_tree_iterator(const avl_tree_iterator & other);

	~avl_tree_iterator();

	avl_tree_iterator & operator = (const avl_tree_iterator & other);

	bool operator == (const avl_tree_iterator & other);
	bool operator != (const avl_tree_iterator & other);

	avl_tree_iterator & operator ++ ();
	avl_tree_iterator & operator ++ (int);

	avl_tree_iterator & operator -- ();
	avl_tree_iterator & operator -- (int);

	const T & operator * () const;
	const T * operator -> () const;

	void swap(avl_tree_iterator & other);

 private:
	avl_tree<T, C, A> * container;
	avl_tree_node<T> * current;
	avl_iterator_orientation orientation;
};

template <typename T, typename C, typename A>
void swap(avl_tree_iterator<T, C, A> & a, avl_tree_iterator<T, C, A> & b)
	{ a.swap(b); }

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
	avl_tree()
	  : root(nullptr)
	  , minimum(nullptr)
	  , maximum(nullptr)
	  , node_count(0)
		{ }

	//
	// Destructor
	//
	~avl_tree() { destroy_tree(); }

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
	bool empty() const { return (root == nullptr); }

	size_type size() const { return node_count; }

	size_type max_size() const;

	//
	// Modifiers
	//
	void clear() { destroy_tree(); }

	// insert() // emplace() // emplace_hint() // erase()
	void insert(const value_type & val);

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

 private:
	typedef avl_tree_node<T> node_type;

	node_type * root, * minimum, * maximum;
	unsigned long node_count;

	void destroy_tree();
};

//////////////////////////////////////////////////////////////////////
template <typename T, typename Comp, typename Alloc>
void avl_tree<T, Comp, Alloc>::insert(const value_type & val)
{
	node_type * n = new node_type(val);

	if (root == nullptr)
		root = n;
	else
	{
		node_type * p = root;
		node_type * last = nullptr;

		while (last != p)
		{
			last = p;
			if (n->value < p->value)
			{
				if (p->left != nullptr) p = p->left;
			} else
			{
				if (p->right != nullptr) p = p->right;
			}
		}

		if (n->value < p->value)
			p->left = n;
		else
			p->right = n;

		n->parent = p;
	}

	++node_count;

	if (minimum == nullptr)
		minimum = n;
	else if (minimum->left != nullptr)
		minimum = minimum->left;

	if (maximum == nullptr)
		maximum = n;
	else if (maximum->right != nullptr)
		maximum = maximum->right;
}

//////////////////////////////////////////////////////////////////////
template <typename T, typename Comp, typename Alloc>
void avl_tree<T, Comp, Alloc>::destroy_tree()
{
	node_type * p = root;
	root = maximum = minimum = nullptr;

	while (p != nullptr)
	{
		if (p->left != nullptr)
		{
			p = p->left;
			p->parent->left = nullptr;
		} else if (p->right != nullptr)
		{
			p = p->right;
			p->parent->right = nullptr;
		} else
		{
			node_type * tmp = p;
			p = p->parent;
			delete tmp;
			--node_count;
		}
	}

	assert(node_count == 0);
}

#endif // GUARD_AVL_TREE_H

