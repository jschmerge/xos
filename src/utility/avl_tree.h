#ifndef GUARD_AVL_TREE_H
#define GUARD_AVL_TREE_H 1

#include <memory>
#include <algorithm>

#include <cstdio>

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
	  : value(std::forward<T>(_value))
	  , parent(nullptr)
	  , left(nullptr)
	  , right(nullptr)
	  , balance(0)
		{ }
};


//
// TODO - This class needs noexcept specifications
//
template <typename T,
          typename Compare = std::less<T>,
          typename Allocator = std::allocator<T>>
class avl_tree
{
	typedef avl_tree_node<T> node_type;
	node_type * root;
	unsigned long node_count;

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
	  , node_count(0)
		{ }

	//
	// Destructor
	//
	~avl_tree() { }

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
	bool empty() const
		{ return (root == nullptr); }

	size_type size() const
		{ return node_count; }

	size_type max_size() const;

	//
	// Modifiers
	//
	void clear();

	// insert() // emplace() // emplace_hint() // erase()
	void insert(const value_type & val)
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
					printf("going left\n");
					if (p->left != nullptr)
						p = p->left;
				} else
				{
					printf("going right\n");
					if (p->right != nullptr)
						p = p->right;
				}
			}

			if (n->value < p->value)
			{
				p->left = n;
			} else
			{
				p->right = n;
			}

			n->parent = p;
		}
	}

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

