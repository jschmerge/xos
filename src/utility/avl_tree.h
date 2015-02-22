#ifndef GUARD_AVL_TREE_H
#define GUARD_AVL_TREE_H 1

#include <memory>
#include <algorithm>
#include <iterator>

#include <cstdio>
#include <cassert>

// forward declaration
template <typename T, typename C, typename A> class avl_tree;

//////////////////////////////////////////////////////////////////////
template <typename T>
struct avl_tree_node
{
	typedef T value_type;

	avl_tree_node * parent, * left, * right;
	typename std::aligned_storage<sizeof(T), alignof(T)>::type storage;

	static constexpr uintptr_t address_mask = ~static_cast<uintptr_t>(3);
	static constexpr uintptr_t balance_mask = static_cast<uintptr_t>(3);

	value_type & value() noexcept
		{ return *value_address(); }

	value_type * value_address() noexcept
		{ return reinterpret_cast<value_type *>(&storage); }

	const value_type & value() const noexcept
		{ return *value_address(); }

	const value_type * value_address() const noexcept
		{ return reinterpret_cast<const value_type *>(&storage); }

	avl_tree_node * parent_node() const noexcept
		{ return reinterpret_cast<avl_tree_node*>(
		           reinterpret_cast<uintptr_t>(parent) & address_mask); }

	void set_parent(avl_tree_node * p) noexcept
	{
		parent = reinterpret_cast<avl_tree_node*>(
		             (reinterpret_cast<uintptr_t>(p) & address_mask)
		           | (reinterpret_cast<uintptr_t>(parent) & balance_mask));
	}

	void set_parent(avl_tree_node * p, int _balance) noexcept
	{
		parent = reinterpret_cast<avl_tree_node*>(
		             (reinterpret_cast<uintptr_t>(p) & address_mask)
		           | (static_cast<uintptr_t>(_balance + 1) & balance_mask));
	}

	int balance() const noexcept
	{
		return (static_cast<int>( reinterpret_cast<uintptr_t>(parent)
		                        & balance_mask) - 1);
	}

	void set_balance(int b) noexcept
	{
		parent = reinterpret_cast<avl_tree_node*>(
		             (reinterpret_cast<uintptr_t>(parent) & address_mask)
		           | (static_cast<uintptr_t>(b + 1) & balance_mask));
	}
};

//////////////////////////////////////////////////////////////////////
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

	size_t height() const
	{
		const node_type * ptr = current;
		size_t count = 1;
		while (ptr->parent_node() != nullptr)
		{
			++count;
			ptr = ptr->parent_node();
		}
		return count;
	}

	int balance() const { return current->balance(); }

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
			node_type * parent_node = current->parent_node();
			while (parent_node != nullptr && current == parent_node->right)
			{
				current = parent_node;
				parent_node = parent_node->parent_node();
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
			node_type * tmp = current->parent_node();
			while (current == tmp->left)
			{
				current = tmp;
				tmp = tmp->parent_node();
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
		{ return current->value(); }

	const T * operator -> () const noexcept
		{ return &(current->value()); }

	void swap(avl_tree_iterator & other) noexcept
	{
		using std::swap;
		swap(root, other.root);
		swap(current, other.current);
	}

	bool is_leaf_node() noexcept
		{ return (  (current->left == nullptr)
		         && (current->right == nullptr)); }

 private:
	const node_type * root;
	const node_type * current;
};

template <typename T>
void swap(avl_tree_iterator<T> & a, avl_tree_iterator<T> & b) noexcept
	{ a.swap(b); }


//////////////////////////////////////////////////////////////////////
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
	static_assert(alignof(node_type) >= 4, "Alignment too small");

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
	using node_alloc_traits = typename alloc_traits::template rebind_traits<node_type>;

	node_type * root, * minimum, * maximum;
	size_type node_count;

	typename node_alloc_traits::allocator_type node_allocator;
	key_compare compare;

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
	  , node_allocator(a)
	  , compare(c)
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

	allocator_type get_allocator() const noexcept
		{ return allocator_type{node_allocator}; }

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

	reverse_iterator rend() noexcept
		{ return reverse_iterator(begin()); }
	const_reverse_iterator rend() const noexcept
		{ return const_reverse_iterator(begin()); }
	const_reverse_iterator crend() const noexcept
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

 private:
	template <typename ... Args>
	node_type * construct_node(Args && ... args)
	{
		node_type * n = node_alloc_traits::allocate(node_allocator, 1);

		n->left = n->right = nullptr;
		n->set_parent(nullptr);
		n->set_balance(0);

		node_alloc_traits::construct(node_allocator,
		                             n->value_address(),
		                             std::forward<Args>(args)...);

		return n;
	}

	void destroy_node(node_type * n)
	{
		node_alloc_traits::destroy(node_allocator, n->value_address());
		node_alloc_traits::deallocate(node_allocator, n, 1);
	}

 public:

	///
	/// modifiers - emplace
	///
	template <typename ... Args>
	std::pair<iterator, bool> emplace(Args && ... args)
	{
		node_type * n = construct_node(std::forward<Args>(args)...);

		node_type * ret = insert_node(n);

		if (ret == n)
		{
			return std::make_pair(iterator(root, n), true);
		} else
		{
			destroy_node(n);
			return std::make_pair(iterator(root, ret), false);
		}
	}

	template <typename ... Args>
	iterator emplace_hint(const_iterator, Args && ... args)
	{
		node_type * n = construct_node(std::forward<Args>(args)...);
		node_type * ret = nullptr;

		if (__builtin_expect(root == nullptr, 0))
		{
			ret = root = maximum = minimum = n;
			++node_count;
		} else if (compare(maximum->value(), n->value()))
		{
			maximum->right = n;
			n->set_parent(maximum);
			maximum = n;
			ret = n;
			rebalance_from(n);
			++node_count;
		} else if (compare(n->value(), minimum->value()))
		{
			minimum->left = n;
			n->set_parent(minimum);
			minimum = n;
			ret = n;
			rebalance_from(n);
			++node_count;
		} else
		{
			ret = insert_node(n);
		}

		if (n != ret) destroy_node(n);
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
#if 0
	iterator erase(const_iterator position)
	{
		if (position == end())
			return position;

		iterator rc(position);
		++rc;

		return rc;
	}

	iterator erase(const_iterator first, const_iterator last)
	{
		iterator i;
		for (i = first; i != last; i = erase(first)) { }
		return i;
	}

	size_type erase(const key_type & k)
	{
		size_type count = 1;
		iterator i = find(k);

		if (i != end())
			erase(i);
		else
			count = 0;

		return count;
	}
#endif

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


	void dump(node_type * n = nullptr, int level = 0)
	{
		if (n == nullptr) n = root;

		if (n == nullptr) return;

		printf("%-10d", n->value());

		if (n->right != nullptr)
			dump(n->right, level + 1);
		else
			printf("-(nil)\n");

		printf("%*s", (level + 1) * 10, "");

		if (n->left != nullptr)
			dump(n->left, level + 1);
		else
			printf("`(nil)\n");
	}

 private:

	node_type * insert_node(node_type * n);
	void destroy_tree() noexcept;
	void rebalance_from(node_type * n);
	void rotate_right(node_type * node);
	void rotate_left(node_type * node);
};

//////////////////////////////////////////////////////////////////////
template <typename T, typename C, typename A>
void avl_tree<T,C,A>::rotate_right(typename avl_tree<T,C,A>::node_type * node)
{
	node_type *  subtree_parent = node->parent_node();
	node_type *  pivot = node->left;
	node_type *  new_right = node;

	( (subtree_parent == nullptr)    ? root : (
	  (subtree_parent->left == node) ? subtree_parent->left :
	                                   subtree_parent->right ) ) = pivot;

	pivot->set_parent(subtree_parent);

	// move pivot's right subtree under left side of new_right
	new_right->left = pivot->right;
	if (new_right->left != nullptr)
		new_right->left->set_parent(new_right);


	// move new_right under right side of pivot
	pivot->right = new_right;
	new_right->set_parent(pivot);
}

//////////////////////////////////////////////////////////////////////
template <typename T, typename C, typename A>
void avl_tree<T,C,A>::rotate_left(typename avl_tree<T,C,A>::node_type * node)
{
	node_type *  subtree_parent = node->parent_node();
	node_type *  pivot = node->right;
	node_type *  new_left = node;

	( (subtree_parent == nullptr)    ? root : (
	  (subtree_parent->left == node) ? subtree_parent->left :
	                                   subtree_parent->right ) ) = pivot;

	pivot->set_parent(subtree_parent);

	// move pivot's left subtree under right side of new_left
	new_left->right = pivot->left;
	if (new_left->right != nullptr)
		new_left->right->set_parent(new_left);

	// move new_right under right side of pivot
	pivot->left = new_left;
	new_left->set_parent(pivot);
}

//////////////////////////////////////////////////////////////////////
template <typename T, typename C, typename A>
void avl_tree<T,C,A>::rebalance_from(typename avl_tree<T,C,A>::node_type * n)
{
	node_type * last = n;
	for (node_type * current = n->parent_node(); current != nullptr;
	     last = current, current = current->parent_node())
	{
		int tmp_balance = current->balance();
		if (current->left == last)
			--tmp_balance;
		else
			++tmp_balance;

		if (tmp_balance == 0)
		{
			current->set_balance(0);
			break;
		} else if (tmp_balance > 1)
		{
			if (current->right->balance() == 1)
			{
				current->set_balance(0);
				current->right->set_balance(0);
			} else
			{
				current->set_balance(
				  std::min(0, -(current->right->left->balance())));
				current->right->set_balance(
				  std::max(0, -(current->right->left->balance())));

				current->right->left->set_balance(0);

				rotate_right(current->right);
			}

			rotate_left(current);
			break;
		} else if (tmp_balance < -1)
		{
			if (current->left->balance() == -1)
			{
				current->set_balance(0);
				current->left->set_balance(0);
			} else
			{
				current->set_balance(
				  std::max(0, -(current->left->right->balance())));
				current->left->set_balance(
				  std::min(0, -(current->left->right->balance())));

				current->left->right->set_balance(0);

				rotate_left(current->left);
			}

			rotate_right(current);
			break;
		} else
		{
			current->set_balance(tmp_balance);
		}
	}
}

//////////////////////////////////////////////////////////////////////
template <typename T, typename C, typename A>
  typename avl_tree<T,C,A>::node_type *
  avl_tree<T,C,A>::insert_node(typename avl_tree<T,C,A>::node_type * n)
{
	node_type * current = root;
	node_type * parent = nullptr;
	node_type ** child_link = &root;

	while (current != nullptr)
	{
		parent = current;

		if (compare(n->value(), current->value()))
		{
			child_link = &(current->left);
			current = current->left;
		} else if (compare(current->value(), n->value()))
		{
			child_link = &(current->right);
			current = current->right;
		} else
		{
			return current;
		}
	}

	*child_link = n;

	n->set_parent(parent);

	if (parent == nullptr)
	{
		minimum = maximum = n;
	}

//	dump();
	rebalance_from(n);

	if (child_link == &(minimum->left))
		minimum = minimum->left;
	else if (child_link == &(maximum->right))
		maximum = maximum->right;

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
			p->parent_node()->left = nullptr;
		} else if (p->right != nullptr)
		{
			p = p->right;
			p->parent_node()->right = nullptr;
		} else
		{
			node_type * tmp = p;
			p = p->parent_node();
			destroy_node(tmp);
			--node_count;
		}
	}

	assert(node_count == 0);
}

#endif // GUARD_AVL_TREE_H
