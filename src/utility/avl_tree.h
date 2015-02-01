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

	explicit avl_tree_node(const value_type & _value)
	  : value(_value)
	  , parent(nullptr)
	  , left(nullptr)
	  , right(nullptr)
	  , balance(0)
		{ }

	explicit avl_tree_node(value_type && _value)
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

template <typename T>
class avl_tree_iterator
  : public std::iterator<std::bidirectional_iterator_tag, T>
{
	typedef avl_tree_node<T> node_type;
 public:
	avl_tree_iterator()
	  : root(nullptr)
	  , current(nullptr)
		{ }

	avl_tree_iterator(const avl_tree_iterator & other)
	  : root(other.root)
	  , current(other.current)
		{ }

	avl_tree_iterator(const node_type * _root, const node_type * _start)
	  : root(_root)
	  , current(_start)
		{ }

	~avl_tree_iterator() = default;

	avl_tree_iterator & operator = (const avl_tree_iterator & other)
	{
		if (this != &other)
		{
			root = other.root;
			current = other.current;
		}
		return *this;
	}

	bool operator == (const avl_tree_iterator & other)
		{ return ( (root == other.root) && (current == other.current) ); }

	bool operator != (const avl_tree_iterator & other)
		{ return !(*this == other); }

	avl_tree_iterator & operator ++ ()
	{
		if (current->right != nullptr)
		{
			current = current->right;
			while (current->left != nullptr)
				current = current->left;
		} else
		{
			node_type * tmp_parent = current->parent;
			while (tmp_parent != nullptr && current == tmp_parent->right)
			{
				current = tmp_parent;
				tmp_parent = tmp_parent->parent;
			}

			if (current->right != tmp_parent)
				current = tmp_parent;
		}

		return *this;
	}

	avl_tree_iterator & operator ++ (int);

	avl_tree_iterator & operator -- ();
	avl_tree_iterator & operator -- (int);

	const T & operator * () const
		{ return current->value; }

	const T * operator -> () const
		{ return &(current->value); }

	void swap(avl_tree_iterator & other);

 private:
	const node_type * root;
	const node_type * current;
};

template <typename T>
void swap(avl_tree_iterator<T> & a, avl_tree_iterator<T> & b)
	{ a.swap(b); }

//
// TODO - This class needs noexcept specifications
//
template <typename T,
          typename Compare = std::less<T>,
          typename Allocator = std::allocator<T>>
class avl_tree
{
	typedef avl_tree_node<T> node_type;

	node_type * root, * minimum, * maximum;
	unsigned long node_count;

	void destroy_tree();

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
	typedef avl_tree_iterator<T>
	                           iterator;
	typedef avl_tree_iterator<T>
	                           const_iterator;
	typedef std::reverse_iterator<iterator>
	                           reverse_iterator;
	typedef std::reverse_iterator<const_iterator>
	                           const_reverse_iterator;

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

	//
	// iterators
	//
	iterator begin()
		{ return iterator(root, minimum); }
	
	iterator end()
		{ return iterator(root, nullptr); }
	
	//
	// Capacity
	//
	bool empty() const { return (root == nullptr); }

	size_type size() const { return node_count; }

	size_type max_size() const { return size_type(-1); }

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

	// find()

	key_compare key_comp() const { return Compare{}; }
	value_compare value_comp() const { return Compare{}; }
	allocator_type get_allocator() const;



 private:
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

