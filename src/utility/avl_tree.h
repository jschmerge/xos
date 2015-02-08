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

	uint16_t height;
	int8_t balance;

	explicit avl_tree_node(const value_type & _value)
	  : value(_value)
	  , parent(nullptr)
	  , left(nullptr)
	  , right(nullptr)
	  , height(1)
	  , balance(0)
		{ }

  template <typename ... Args>
	avl_tree_node(Args && ... args)
	  : value(std::forward<Args>(args)...)
	  , parent(nullptr)
	  , left(nullptr)
	  , right(nullptr)
	  , height(1)
	  , balance(0)
		{ }

	~avl_tree_node()
		{ assert(left == nullptr && right == nullptr); }
};

template <typename T>
class avl_tree_iterator
  : public std::iterator<std::bidirectional_iterator_tag,
                         T, std::ptrdiff_t, const T *, const T &>
{
	typedef avl_tree_node<T> node_type;
 public:
	avl_tree_iterator() noexcept
	  : root(nullptr)
	  , current(nullptr)
		{ }

	avl_tree_iterator(const avl_tree_iterator & other) noexcept
	  : root(other.root)
	  , current(other.current)
		{ }

	avl_tree_iterator(const node_type * _root, const node_type * _start)
	  noexcept
	  : root(_root)
	  , current(_start)
		{ }

	~avl_tree_iterator() = default;

	avl_tree_iterator & operator = (const avl_tree_iterator & other) noexcept
	{
		if (this != &other)
		{
			root = other.root;
			current = other.current;
		}
		return *this;
	}

	int height() const { return current->height; }
	bool operator == (const avl_tree_iterator & other) noexcept
		{ return ( (root == other.root) && (current == other.current) ); }

	bool operator != (const avl_tree_iterator & other) noexcept
		{ return !(*this == other); }

	// This function is based on and adapted from the SGI rbtree implementation
	// Since we are using a different representation for end(), the handling
	// of that case is slightly different
	avl_tree_iterator & operator ++ () noexcept
	{
		if (current->right != nullptr)
		{
			current = current->right;
			while (current->left != nullptr)
				current = current->left;
		} else
		{
			node_type * parent_node = current->parent;
			while (parent_node != nullptr && current == parent_node->right)
			{
				current = parent_node;
				parent_node = parent_node->parent;
			}

			current = parent_node;
		}
		return *this;
	}

	avl_tree_iterator & operator -- () noexcept
	{
		if (current == nullptr)
		{
			// if the iterator currently contains the value of end()
			current = root;
			while (current->right != nullptr)
				current = current->right;
		} else if (current->left != nullptr)
		{
			node_type * tmp = current->left;
			while (tmp->right != 0)
				tmp = tmp->right;
			current = tmp;
		}
		else
		{
			node_type * tmp = current->parent;
			while (current == tmp->left)
			{
				current = tmp;
				tmp = tmp->parent;
			}
			current = tmp;
		}
		return *this;
	}

	avl_tree_iterator & operator ++ (int) noexcept
		{ avl_tree_iterator tmp = *this; ++(*this); return tmp; }

	avl_tree_iterator & operator -- (int) noexcept
		{ avl_tree_iterator tmp = *this; --(*this); return tmp; }

	const T & operator * () const noexcept
	{
 		return current->value;
	}

	const T * operator -> () const noexcept
		{ return &(current->value); }

	void swap(avl_tree_iterator & other) noexcept
	{
		using std::swap;
		swap(root, other.root);
		swap(current, other.current);
	}

 private:
	const node_type * root;
	const node_type * current;
};

template <typename T>
void swap(avl_tree_iterator<T> & a, avl_tree_iterator<T> & b) noexcept
	{ a.swap(b); }

//
// TODO - This class needs noexcept specifications
//
template <typename T,
          typename Compare = std::less<T>,
          typename Allocator = std::allocator<T>>
class avl_tree
{
 private:
	typedef std::allocator_traits<Allocator>       alloc_traits;
	typedef avl_tree_node<T>                       node_type;

 public:
	///
	/// Type definitions
	///
	typedef T                                      key_type;
	typedef T                                      value_type;
	typedef std::size_t                            size_type;
	typedef std::ptrdiff_t                         difference_type;
	typedef Compare                                key_compare;
	typedef Compare                                value_compare;
	typedef Allocator                              allocator_type;
	typedef value_type                           & reference;
	typedef const value_type                     & const_reference;
	typedef typename alloc_traits::pointer         pointer;
	typedef typename alloc_traits::const_pointer   const_pointer;
	typedef avl_tree_iterator<T>                   iterator;
	typedef avl_tree_iterator<T>                   const_iterator;
	typedef std::reverse_iterator<const_iterator>  reverse_iterator;
	typedef std::reverse_iterator<const_iterator>  const_reverse_iterator;

 private:

	node_type * root, * minimum, * maximum;
	size_type node_count;

	key_compare compare;
	allocator_type allocator;

	node_type * insert_node(node_type * n);
	void destroy_tree() noexcept;
	void rebalance_from(node_type * start);

 public:

	///
	/// Constructors
	///
	avl_tree() : avl_tree(key_compare()) { }

	explicit
	avl_tree(const key_compare & c, const allocator_type & a = allocator_type())
	  : root(nullptr)
	  , minimum(nullptr)
	  , maximum(nullptr)
	  , node_count(0)
	  , compare(c)
	  , allocator(a)
		{ }
	  
	template <class InputIterator>
	avl_tree(InputIterator first, InputIterator last,
	         const Compare & comp = Compare{},
	         const Allocator & alloc = Allocator{})
	  : avl_tree(comp, alloc)
		{ insert(first, last); }

	template <class InputIterator>
	avl_tree(InputIterator first, InputIterator last, const Allocator & a)
	  : avl_tree(first, last, Compare{}, a) { }

	explicit avl_tree(const Allocator & a)
	  : avl_tree(key_compare(), a) { }

	avl_tree(const avl_tree & other, const allocator_type & a)
	  : avl_tree(other.begin(), other.end(), other.compare, a) { }

	avl_tree(const avl_tree & other)
	  : avl_tree(other, alloc_traits::select_on_container_copy_construction(
	                      other.get_allocator())) { }

	avl_tree(std::initializer_list<value_type> list,
	         const key_compare & c = key_compare{},
	         const allocator_type & a = allocator_type{})
	  : avl_tree(list, c, a) { }

	avl_tree(std::initializer_list<value_type> list, const allocator_type & a)
	  : avl_tree(list, key_compare{}, a) { }

	// TODO
	avl_tree(avl_tree && other, const allocator_type & a);
	avl_tree(avl_tree && other);

	///
	/// Destructor
	///
	~avl_tree() { destroy_tree(); }

	///
	/// Assignment
	/// TODO
	avl_tree & operator = (const avl_tree & other);
	avl_tree & operator = (avl_tree && other);
	avl_tree & operator = (std::initializer_list<value_type> list);

	allocator_type get_allocator() const noexcept { return allocator; }

	///
	/// iteration bounds
	///
	iterator begin() noexcept
		{ return iterator(root, minimum); }
	const_iterator begin() const noexcept
		{ return const_iterator(root, minimum); }
	const_iterator cbegin() const noexcept
		{ return const_iterator(root, minimum); }

	iterator end() noexcept
		{ return iterator(root, nullptr); }
	const_iterator end() const noexcept
		{ return const_iterator(root, nullptr); }
	const_iterator cend() const noexcept
		{ return const_iterator(root, nullptr); }

	reverse_iterator rbegin() noexcept
		{ return reverse_iterator(end()); }
	const_reverse_iterator rbegin() const noexcept
		{ return const_reverse_iterator(end()); }
	const_reverse_iterator crbegin() const noexcept
		{ return const_reverse_iterator(end()); }

	reverse_iterator rend()
		{ return reverse_iterator(begin()); }
	const_reverse_iterator rend() const
		{ return const_reverse_iterator(begin()); }
	const_reverse_iterator crend() const
		{ return const_reverse_iterator(begin()); }
	
	//////
	///
	/// Capacity
	///
	//////
	bool empty() const noexcept { return (root == nullptr); }
	size_type size() const noexcept { return node_count; }
	size_type max_size() const noexcept { return size_type(-1); }

	//////
	///
	/// Modifiers
	///
	//////

	///
	/// modifiers - emplace
	///
	template <typename ... Args>
	std::pair<iterator, bool> emplace(Args && ... args)
	{
		node_type * n = new node_type(std::forward<Args>(args)...);
		node_type * ret = insert_node(n);
		if (ret == n)
			return std::make_pair(iterator(root, n), true);
		else {
			delete n;
			return std::make_pair(iterator(root, ret), false);
		}
	}

	template <typename ... Args>
	iterator emplace_hint(const_iterator, Args && ... args)
	{
		node_type * n = new node_type(std::forward<Args>(args)...);
		node_type * ret = insert_node(n);
		if (n != ret) delete n;
		return iterator(root, ret);
	}

	///
	/// modifiers - insert
	///
	std::pair<iterator, bool> insert(const value_type & val)
		{ return emplace(val); }

	std::pair<iterator, bool> insert(value_type && val)
		{ return emplace(std::forward<value_type>(val)); }

	iterator insert(const_iterator pos, const value_type & value)
		{ return emplace_hint(pos, value); }

	iterator insert(const_iterator pos, value_type && value)
		{ return emplace_hint(pos, std::forward<value_type>(value)); }

	template<class InputIterator>
	  void insert(InputIterator first, InputIterator last)
		{ for (;first != last; ++first) insert(this->end(), *first); }

	void insert(std::initializer_list<value_type> list)
		{ insert(list.begin(), list.end()); }

	///
	/// modifiers - erase
	///
	iterator erase(const_iterator position);

	iterator erase(const_iterator first, const_iterator last);

	size_type erase(const key_type & x);


	// TODO
	void swap(avl_tree &)
	  noexcept(alloc_traits::is_always_equal::value &&
	           noexcept(swap(std::declval<key_compare&>(),
	                         std::declval<key_compare&>())));

	void clear() noexcept { destroy_tree(); }

	///
	/// Observers
	///
	key_compare key_comp() const { return Compare{}; }
	value_compare value_comp() const { return Compare{}; }

	///
	/// Operations
	///
	iterator find(const key_type& x);
	const_iterator find(const key_type& x) const;
	template <class K>
	  iterator find(const K & x);
	template <class K>
	  const_iterator find(const K & x) const;

	size_type count(const key_type& x) const;
	template <class K>
	  size_type count(const K& x) const;

	iterator lower_bound(const key_type & x);
	const_iterator lower_bound(const key_type& x) const;
	template <class K>
	  iterator lower_bound(const K& x);
	template <class K>
	  const_iterator lower_bound(const K& x) const;

	iterator upper_bound(const key_type & x);
	const_iterator upper_bound(const key_type& x) const;
	template <class K>
	  iterator upper_bound(const K& x);
	template <class K>
	  const_iterator upper_bound(const K& x) const;

	typedef std::pair<iterator,iterator> iter_range;
	typedef std::pair<const_iterator,const_iterator> const_iter_range;

	iter_range equal_range(const key_type& x);
	const_iter_range equal_range(const key_type& x) const;
	template <class K>
	  iter_range equal_range(const K& x);
	template <class K>
	  const_iter_range equal_range(const K& x) const;
};

//////////////////////////////////////////////////////////////////////
template <typename T, typename Comp, typename Alloc>
void avl_tree<T, Comp, Alloc>::rebalance_from(
    typename avl_tree<T, Comp, Alloc>::node_type * start)
{
	node_type * p = start;

	int left_height = 0, right_height = 0;

//	printf("Starting with node %d\n", start->value);
	while (p != nullptr)
	{
		left_height  = ((p->left == nullptr)  ? 0 : p->left->height);
		right_height = ((p->right == nullptr) ? 0 : p->right->height);

//		printf("\tleft = %d, right = %d\n", left_height, right_height);
		if (left_height - right_height >= 2)
		{
//			printf("\trebalancing right\n");
		} else if (left_height - right_height <= -2)
		{
//			printf("\trebalancing left\n");
		}

		p = p->parent;
	}
}

//////////////////////////////////////////////////////////////////////
template <typename T, typename Comp, typename Alloc>
  typename avl_tree<T, Comp, Alloc>::node_type *
    avl_tree<T, Comp, Alloc>::insert_node(
      typename avl_tree<T, Comp, Alloc>::node_type * n)
{
	if (root == nullptr)
	{
		root = n;
		minimum = n;
		maximum = n;
	} else
	{
		node_type * current = root;
		node_type * parent = nullptr;
		node_type ** child_link = nullptr;

		while (current != nullptr)
		{
			parent = current;

			if (compare(n->value, current->value))
			{
				child_link = &(current->left);
				current = current->left;
			} else if (compare(current->value, n->value))
			{
				child_link = &(current->right);
				current = current->right;
			} else
			{
				return current;
			}
		}

		*child_link = n;

		n->parent = parent;
		n->height = (parent->height + 1);

		rebalance_from(n);

		if (child_link == &(minimum->left))
			minimum = minimum->left;
		else if (child_link == &(maximum->right))
			maximum = maximum->right;
	}

	++node_count;

	return n;
}

//////////////////////////////////////////////////////////////////////
template <typename T, typename Comp, typename Alloc>
void avl_tree<T, Comp, Alloc>::destroy_tree() noexcept
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
