#ifndef GUARD_AVL_TREE_H
#define GUARD_AVL_TREE_H 1

void gets();

#include <memory>
#include <algorithm>
#include <iterator>

#include <cstdio>
#include <iostream>
#include <iomanip>
#include <cassert>

namespace {
	template <typename...>
	using type_defined = void;

	template<typename K, typename CMP, typename = type_defined<>>
	struct is_transparent : std::false_type { };

	template<typename K, typename CMP>
	struct is_transparent<K, CMP, type_defined<typename CMP::is_transparent>>
	  : std::true_type { };
}

// forward declaration
template <typename T, typename C, typename A> class avl_tree;

//////////////////////////////////////////////////////////////////////
template <typename T>
struct avl_tree_node
{
	typedef T value_type;

	avl_tree_node * parent;
	avl_tree_node * left;
	avl_tree_node * right;

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
template <typename T, typename C, typename A>
class avl_tree_iterator
  : public std::iterator<std::bidirectional_iterator_tag,
                         T, std::ptrdiff_t, const T *, const T &>
{
	typedef avl_tree_node<T> node_type;
 public:
	avl_tree_iterator() noexcept
	  : current(nullptr)
		{ }

	avl_tree_iterator(const avl_tree_iterator & other) noexcept
	  : current(other.current)
		{ }

	~avl_tree_iterator() = default;

	avl_tree_iterator & operator = (const avl_tree_iterator & other) noexcept
	{
		if (this != &other)
			current = other.current;
		return *this;
	}

	size_t height() const noexcept
	{
		const node_type * ptr = current;
		size_t count = 0;
		while (ptr->parent_node() != nullptr)
		{
			++count;
			ptr = ptr->parent_node();
		}
		return count;
	}

	int balance() const noexcept { return current->balance(); }

	bool operator == (const avl_tree_iterator & other) noexcept
		{ return (current == other.current); }

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
			while (current == parent_node->right)
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
		if (current->left != nullptr)
		{
			node_type * tmp = current->left;
			while (tmp->right != 0)
				tmp = tmp->right;
			current = tmp;
		} else
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
		swap(current, other.current);
	}

	bool is_leaf_node() noexcept
		{ return (  (current->left == nullptr)
		         && (current->right == nullptr)); }

 private:
	avl_tree_iterator(const node_type * _start) noexcept
	  : current(const_cast<node_type*>(_start)) { }

	friend class avl_tree<T, C, A>;
	node_type * current;
};

template <typename T, typename C, typename A>
void swap(avl_tree_iterator<T,C,A> & a, avl_tree_iterator<T,C,A> & b) noexcept
	{ a.swap(b); }

//////////////////////////////////////////////////////////////////////
//
// TODO - This class needs noexcept specifications
//
template <typename T, typename Compare = std::less<T>,
          typename Allocator = std::allocator<T>>
class avl_tree
{
 private:
	typedef avl_tree_node<T>                       node_type;
	typedef std::allocator_traits<Allocator>       alloc_traits;
	typedef typename alloc_traits::template rebind_traits<node_type>
	                                               node_alloc_traits;
	typedef typename node_alloc_traits::allocator_type
	                                               node_alloc;

	// The avl_tree_node/node_type structure places the balance accounting
	// within the least significant 2 bits of the 'parent' pointer of the
	// node... This is a necessary check to make sure those bits will
	// be available for the space optimization
	static_assert(alignof(node_type) >= 4, "Alignment too small");

 public:
	template <typename U, typename V>
	static constexpr
	bool compare_is_noexcept(const U & u = U{}, const V & v = V{})
		{ return (noexcept(compare(u, v)) && noexcept(compare(v, u))); }

	///
	/// Required type definitions
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
	typedef avl_tree_iterator<T,Compare,Allocator> iterator;
	typedef avl_tree_iterator<T,Compare,Allocator> const_iterator;
	typedef std::reverse_iterator<const_iterator>  reverse_iterator;
	typedef std::reverse_iterator<const_iterator>  const_reverse_iterator;

 private:
	// Data members
	node_type   sentinel;
	node_type * minimum;
	node_type * maximum;
	size_type   node_count;
	node_alloc  node_allocator;
	key_compare compare;

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

	node_type * leftmost_child(node_type * n) noexcept
	{
		while (n->left != nullptr) { n = n->left; }
		return n;
	}

	node_type * rightmost_child(node_type * n) noexcept
	{
		while (n->right != nullptr) { n = n->right; }
		return n;
	}

	node_type * insert_node(node_type * n);

	void destroy_tree() noexcept;

	// rebalancing routines
	void rebalance_after_insert(node_type * n) noexcept;
	bool rotate_right(node_type * node) noexcept;
	bool rotate_left(node_type * node) noexcept;
	bool double_rotate_right(node_type * node) noexcept;
	bool double_rotate_left(node_type * node) noexcept;

	// Deletion helpers
	void delete_node(node_type * target) noexcept;
	void swap_with_neighbor(node_type * target, node_type * neighbor) noexcept;
	std::pair<node_type *, int> delete_leaf_node(node_type * target) noexcept;
	std::pair<node_type *, int> delete_link_node(node_type * target) noexcept;
	std::pair<node_type *, int> delete_inner_node(node_type * target) noexcept;

	template <typename K>
	node_type *
	find_impl(const K & value, node_type  * starting_point,
	          node_type  * & last, node_type ** & child_link)
	  noexcept(compare_is_noexcept<K, T>())
	{
		node_type * current = starting_point;

		while (current != nullptr)
		{
			if (compare(value, current->value()))
			{
				last = current;
				child_link = &(current->left);
				current = current->left;
			} else if (compare(current->value(), value))
			{
	
				last = current;
				child_link = &(current->right);
				current = current->right;
			} else
				break;
		}

		return current;
	}

	template <typename K>
	const node_type *
	lower_bound_impl(const K & value,
	                 const node_type * current,
	                 const node_type * last) const
	  noexcept(compare_is_noexcept<K, T>())
	{
		while (current != nullptr)
		{
			if ( ! compare(current->value(), value))
			{
				last = current;
				current = current->left;
			} else
			{
				current = current->right;
			}
		}

		return last;
	}

	template <typename K>
	const node_type *
	upper_bound_impl(const K & value,
	                 const node_type * current,
	                 const node_type * last) const
	  noexcept(compare_is_noexcept<K, T>())
	{
		while (current != nullptr)
		{
			if (compare(value, current->value()))
			{
				last = current;
				current = current->left;
			} else
			{
				current = current->right;
			}
		}

		return last;
	}

	template <typename K>
	void equal_range_impl(const K & value,
	                      const node_type * & begin,
	                      const node_type * & end) const
	  noexcept(compare_is_noexcept<K, T>())
	{
		node_type * current = sentinel.left;
		const node_type * last = &sentinel;
		begin = &sentinel;
		end = &sentinel;

		while (current != nullptr)
		{
			if (compare(value, current->value()))
			{
				last = current;
				current = current->left;
			} else if (compare(current->value(), value))
			{
				last = current;
				current = current->right;
			} else
			{
				begin = lower_bound_impl(value, current, last);
				end = upper_bound_impl(value, current, last);
				current = nullptr;
			}
		}
	}

 public:
	///
	/// Constructors
	///
	avl_tree() noexcept( noexcept(key_compare{})
	                  && noexcept(avl_tree(key_compare{})))
	  : avl_tree(key_compare()) { }

	explicit
	avl_tree(const key_compare & c,
	         const allocator_type & a = allocator_type())
	  : sentinel()
	  , minimum(&sentinel)
	  , maximum(&sentinel)
	  , node_count(0)
	  , node_allocator(a)
	  , compare(c)
	{
		sentinel.set_parent(nullptr, 0);
		sentinel.left = sentinel.right = nullptr;
	}
	  
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
	  : avl_tree(list.begin(), list.end(), c, a) { }

	avl_tree(std::initializer_list<value_type> list, const allocator_type & a)
	  : avl_tree(list.begin(), list.end(), key_compare{}, a) { }

	// TODO
	avl_tree(avl_tree && other, const allocator_type & a);
	avl_tree(avl_tree && other);

	///
	/// Destructor
	///
	~avl_tree() { destroy_tree(); }

	///
	/// Assignment
	///
	avl_tree & operator = (const avl_tree & other)
	{
		if (this != &other)
		{
			this->clear();
			this->insert(other.begin(), other.end());
		}
		return *this;
	}

	/// TODO
	avl_tree & operator = (avl_tree && other) noexcept;

	avl_tree & operator = (std::initializer_list<value_type> list)
	{
		this->clear();
		this->insert(list.begin(), list.end());
		return *this;
	}

	///
	/// iteration bounds
	///
	iterator begin() noexcept
		{ return iterator(minimum); }
	const_iterator begin() const noexcept
		{ return const_iterator(minimum); }
	const_iterator cbegin() const noexcept
		{ return const_iterator(minimum); }

	iterator end() noexcept
		{ return iterator(&sentinel); }
	const_iterator end() const noexcept
		{ return const_iterator(&sentinel); }
	const_iterator cend() const noexcept
		{ return const_iterator(&sentinel); }

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
	bool empty() const noexcept { return (sentinel.left == nullptr); }
	size_type size() const noexcept { return node_count; }
	size_type max_size() const noexcept { return size_type(-1); }

	//////
	///
	/// Modifiers
	///
	//////

	///
	/// Modifiers - emplace
	///
	template <typename ... Args>
	std::pair<iterator, bool> emplace(Args && ... args)
	{
		node_type * n = construct_node(std::forward<Args>(args)...);

		node_type * ret = insert_node(n);

		if (ret == n)
		{
			return std::make_pair(iterator(n), true);
		} else
		{
			destroy_node(n);
			return std::make_pair(iterator(ret), false);
		}
	}

	template <typename ... Args>
	iterator emplace_hint(const_iterator, Args && ... args)
	{
		node_type * n = construct_node(std::forward<Args>(args)...);
		node_type * ret = nullptr;

		if (__builtin_expect(sentinel.left == nullptr, 0))
		{
			ret = sentinel.left = maximum = minimum = n;
			n->set_parent(&sentinel);
			++node_count;
		} else if (compare(maximum->value(), n->value()))
		{
			maximum->right = n;
			n->set_parent(maximum);
			maximum = n;
			ret = n;
			rebalance_after_insert(n);
			++node_count;
		} else if (compare(n->value(), minimum->value()))
		{
			minimum->left = n;
			n->set_parent(minimum);
			minimum = n;
			ret = n;
			rebalance_after_insert(n);
			++node_count;
		} else
		{
			ret = insert_node(n);
		}

		if (n != ret) destroy_node(n);
		return iterator(ret);
	}

	///
	/// Modifiers - insert
	///

	std::pair<iterator, bool> insert(const value_type & val)
	{
		bool inserted = false;
		node_type  * parent = &sentinel;
 		node_type ** child_link = &(sentinel.left);
		node_type * n = find_impl(val, sentinel.left, parent, child_link);

		if (n == nullptr)
		{
			inserted = true;
			n = construct_node(val);
			*child_link = n;
			n->set_parent(parent);

			if (parent == &sentinel)
				minimum = maximum = n;

			rebalance_after_insert(n);

			if (child_link == &(minimum->left))
				minimum = minimum->left;
			else if (child_link == &(maximum->right))
				maximum = maximum->right;

			++node_count;
		}

		return std::make_pair(iterator{n}, inserted);
	}

	std::pair<iterator, bool> insert(value_type && value)
	{
		bool inserted = false;
		node_type  * parent = &sentinel;
 		node_type ** child_link = &(sentinel.left);
		node_type * n = find_impl(value, sentinel.left, parent, child_link);

		if (n == nullptr)
		{
			inserted = true;
			n = construct_node(std::forward<value_type>(value));
			*child_link = n;
			n->set_parent(parent);

			if (parent == &sentinel)
				minimum = maximum = n;

			rebalance_after_insert(n);

			if (child_link == &(minimum->left))
				minimum = minimum->left;
			else if (child_link == &(maximum->right))
				maximum = maximum->right;

			++node_count;
		}

		return std::make_pair(iterator{n}, inserted);
	}

	iterator insert(const_iterator pos, const value_type & value)
	{
		node_type * current = pos.current;
		node_type * inserted = current;

		if (__builtin_expect(node_count == 0, 0))
		{
			return insert(value).first;
		} else if (current == &sentinel) // pos == end()
		{
			if (compare(maximum->value(), value))
			{
				// insert at max
				inserted = construct_node(value);
				inserted->set_parent(maximum);
				maximum->right = inserted;
				maximum = inserted;
				rebalance_after_insert(inserted);
				++node_count;
			} else
			{
				return insert(value).first;
			}
		} else if (current == minimum)
		{
			if (compare(value, minimum->value()))
			{
				// insert at min
				inserted = construct_node(value);
				inserted->set_parent(minimum);
				minimum->left = inserted;
				minimum = inserted;
				rebalance_after_insert(inserted);
				++node_count;
			} else
			{
				return insert(value).first;
			}
		} else if (compare(value, current->value()))
		{
			node_type * prev = std::prev(pos).current;
			if (!compare(value, prev->value()))
			{
				assert(  (current->left == nullptr)
				      || (prev->right == nullptr) );

				if (current->left == nullptr)
				{
					inserted = construct_node(value);
					inserted->set_parent(current);
					current->left = inserted;
					rebalance_after_insert(inserted);
					++node_count;
				} else if (prev->right == nullptr)
				{
					inserted = construct_node(value);
					inserted->set_parent(prev);
					prev->right = inserted;
					rebalance_after_insert(inserted);
					++node_count;
				}
			}
		} else if (compare(current->value(), value))
		{
			return insert(value).first;
		}

		return inserted; // equivalent to pos
	}
#if 0

	iterator insert(const_iterator pos, value_type && value)
		{ return emplace_hint(pos, std::forward<value_type>(value)); }
#endif

	template<class InputIterator>
	  void insert(InputIterator first, InputIterator last)
		{ for (;first != last; ++first) insert(this->end(), *first); }

	void insert(std::initializer_list<value_type> list)
		{ insert(list.begin(), list.end()); }

	///
	/// Modifiers - erase
	///
	iterator erase(const_iterator position)
	{
		iterator rc(position);

		if (position.current != &sentinel)
		{
			++rc;
			delete_node(position.current);
		}

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
		size_type count = 0;
		node_type * last = &sentinel;
		node_type ** child_link = &(sentinel.left);

		node_type * target = find_impl(k, sentinel.left, last, child_link);

		if (target != nullptr)
		{
			++count;
			delete_node(target);
		}

		return count;
	}

	// TODO
	void swap(avl_tree &)
	  noexcept(alloc_traits::is_always_equal::value &&
	           noexcept(swap(std::declval<key_compare&>(),
	                         std::declval<key_compare&>())));

	void clear() noexcept { destroy_tree(); }

	///
	/// Observers
	///
	key_compare key_comp() const
	  noexcept(std::is_nothrow_constructible<key_compare>::value)
		{ return Compare{}; }

	value_compare value_comp() const
	  noexcept(std::is_nothrow_constructible<key_compare>::value)
		{ return Compare{}; }

	allocator_type get_allocator() const
	  noexcept(noexcept(allocator_type{node_allocator}))
		{ return allocator_type{node_allocator}; }

	//////
	///
	/// Operations
	///
	//////

	///
	/// returns an iterator pointing to an element with the key
	/// equivalent to k, or a.end() if such an element is not found
	///
	/// Complexity: log(tree.size())
	///
	iterator find(const key_type & value)
	{
		node_type ** child = &(sentinel.left);
		node_type * last = &sentinel;
		const node_type * target = find_impl(value, sentinel.left, last, child);
		return (target ?  iterator(target) : end());
	}

	const_iterator find(const key_type & value) const
	{
		node_type ** child = &(sentinel.left);
		node_type * last = &sentinel;
		const node_type * target = find_impl(value, sentinel.left, last, child);
		return (target ?  iterator(target) : end());
	}


	template <typename K>
	typename std::enable_if<is_transparent<K, key_compare>{}, iterator>::type
	find(const K & value)
	{
		node_type ** child = &(sentinel.left);
		node_type * last = &sentinel;
		const node_type * target = find_impl(value, sentinel.left, last, child);
		return (target ?  iterator(target) : end());
	}


	template <typename K>
	typename std::enable_if<is_transparent<K, key_compare>{},
	                        const_iterator>::type
	find(const K & value) const
	{
		node_type ** child = &(sentinel.left);
		node_type * last = &sentinel;
		const node_type * target = find_impl(value, sentinel.left, last, child);
		return (target ?  iterator(target) : end());
	}

	///
	/// Returns the number of elements with key equivalent to value
	///
	/// Complexity:  log(tree.size()) + tree.count(value)
	///
	size_type count(const key_type & value) const
	{
		auto r = equal_range(value);
		return std::distance(r.first, r.second);
	}

	template <typename K>
	typename std::enable_if<is_transparent<K, key_compare>{}, size_type>::type
	count(const K & value) const
	{
		auto r = equal_range(value);
		return std::distance(r.first, r.second);
	}

	///
	/// Returns an iterator pointing to the first element with key not
	/// less than value, or a.end() if such an element is not found.
	///
	/// Complexity: log(tree.size())
	///
	iterator lower_bound(const key_type & value)
	{
		const node_type * last = &sentinel, * current = sentinel.left;
		return lower_bound_impl(value, current, last);
	}

	const_iterator lower_bound(const key_type & value) const
	{
		const node_type * last = &sentinel, * current = sentinel.left;
		return lower_bound_impl(value, current, last);
	}

	template <typename K>
	typename std::enable_if<is_transparent<K, key_compare>{}, iterator>::type
	lower_bound(const K & value)
	{
		const node_type * last = &sentinel, * current = sentinel.left;
		return lower_bound_impl(value, current, last);
	}

	template <typename K>
	typename std::enable_if<is_transparent<K, key_compare>{},
	                        const_iterator>::type
	lower_bound(const K & value) const
	{
		const node_type * last = &sentinel, * current = sentinel.left;
		return lower_bound_impl(value, current, last);
	}

	///
	/// returns an iterator pointing to the first element with key
	/// greater than k, or a.end() if such an element is not found.
	///
	/// Complexity: logarithmic
	///
	iterator upper_bound(const key_type & value)
	{
		const node_type * last = &sentinel, * current = sentinel.left;
		return upper_bound_impl(value, current, last);
	}

	const_iterator upper_bound(const key_type & value) const
	{
		const node_type * last = &sentinel, * current = sentinel.left;
		return upper_bound_impl(value, current, last);
	}

	template <typename K>
	typename std::enable_if<is_transparent<K, key_compare>{}, iterator>::type
	upper_bound(const K & value)
	{
		const node_type * last = &sentinel, * current = sentinel.left;
		return upper_bound_impl(value, current, last);
	}

	template <typename K>
	typename std::enable_if<is_transparent<K, key_compare>{},
	                        const_iterator>::type
	upper_bound(const K & value) const
	{
		const node_type * last = &sentinel, * current = sentinel.left;
		return upper_bound_impl(value, current, last);
	}

	///
	/// Equivalent to make_pair(a.lower_bound(k), a.upper_bound(k)).
	///
	/// Complexity: logarithmic
	///
	std::pair<iterator, iterator>
	equal_range(const key_type & value)
	{
		const node_type * begin = nullptr, * end = nullptr;
		equal_range_impl(value, begin, end);
		return std::make_pair(iterator{begin}, iterator{end});
	}

	std::pair<const_iterator, const_iterator>
	equal_range(const key_type & value) const
	{
		const node_type * begin = nullptr, * end = nullptr;
		equal_range_impl(value, begin, end);
		return std::make_pair(iterator{begin}, iterator{end});
	}

	template <typename K>
	typename std::enable_if<is_transparent<K, key_compare>{},
	                        std::pair<iterator, iterator>>::type
	equal_range(const K & value)
	{
		const node_type * begin = nullptr, * end = nullptr;
		equal_range_impl(value, begin, end);
		return std::make_pair(iterator{begin}, iterator{end});
	}

	template <typename K>
	typename std::enable_if<is_transparent<K, key_compare>{},
	                        std::pair<const_iterator, const_iterator>>::type
	equal_range(const K & value) const
	{
		const node_type * begin = nullptr, * end = nullptr;
		equal_range_impl(value, begin, end);
		return std::make_pair(iterator{begin}, iterator{end});
	}

	void dump(node_type * n = nullptr, int level = 0);
};

//////////////////////////////////////////////////////////////////////
template <typename T, typename C, typename A>
void avl_tree<T,C,A>::dump(typename avl_tree<T,C,A>::node_type * n, int level)
{
	if (n == nullptr) n = sentinel.left;

	if (n == nullptr) { printf("EMPTY TREE\n"); return; }

	std::cout << "(" << std::setw(2) << n->balance() << ")"
	          << std::setw(10) << std::left << n->value();
	          
	
	//printf("(% 2d)%-10d", n->balance(), n->value());

	if (n->right != nullptr)
		dump(n->right, level + 1);
	else
		printf("-(nil)\n");

	printf("%*s", (level + 1) * 14, "");

	if (n->left != nullptr)
		dump(n->left, level + 1);
	else
		printf("`(nil)\n");

	if (level == 0)
	{
		size_t min = ~0, max = 0;
		for (auto i = begin(); i != end(); ++i)
		{
			if (i.current->left == nullptr || i.current->right == nullptr)
			{
				if (min > i.height()) min = i.height();
				if (max < i.height()) max = i.height();
			}
		}

		printf("min = %zu, max = %zu\n", min, max);
	}
}

//////////////////////////////////////////////////////////////////////
template <typename T, typename C, typename A>
bool avl_tree<T,C,A>::rotate_right(typename avl_tree<T,C,A>::node_type * node)
  noexcept
{
	node_type * subtree_parent = node->parent_node();
	node_type * pivot = node->left;
	node_type * new_right = node;
	bool rc = true;

	( ( subtree_parent->left == node) ? subtree_parent->left :
	                                    subtree_parent->right ) = pivot;

	pivot->set_parent(subtree_parent);

	// move pivot's right subtree under left side of new_right
	new_right->left = pivot->right;
	if (new_right->left != nullptr)
		new_right->left->set_parent(new_right);

	// move new_right under right side of pivot
	pivot->right = new_right;
	new_right->set_parent(pivot);

	if (pivot->balance() < 0)
	{
		pivot->set_balance(0);
		new_right->set_balance(0);
	} else if (pivot->balance() == 0)
	{
		pivot->set_balance(1);
		new_right->set_balance(-1);
		rc = false;
	}

	return rc;
}

//////////////////////////////////////////////////////////////////////
template <typename T, typename C, typename A>
bool avl_tree<T,C,A>::
double_rotate_right(typename avl_tree<T,C,A>::node_type * node)
  noexcept
{
	node_type * subtree_parent = node->parent_node();
	node_type * pivot = node->left->right;
	node_type * new_right = node;
	node_type * new_left = node->left;

	( ( subtree_parent->left == node) ? subtree_parent->left :
	                                    subtree_parent->right ) = pivot;

	pivot->set_parent(subtree_parent);

	new_right->left = pivot->right;
	if (new_right->left != nullptr)
		new_right->left->set_parent(new_right);

	new_left->right = pivot->left;
	if (new_left->right != nullptr)
		new_left->right->set_parent(new_left);

	pivot->left = new_left;
	new_left->set_parent(pivot);

	pivot->right = new_right;
	new_right->set_parent(pivot);

	if (pivot->balance() == -1)
	{
		new_left->set_balance(0);
		new_right->set_balance(1);
		pivot->set_balance(0);
	} else if (pivot->balance() == 0)
	{
		new_left->set_balance(0);
		new_right->set_balance(0);
	} else //if (pivot->balance == 1)
	{
		new_left->set_balance(-1);
		new_right->set_balance(0);
		pivot->set_balance(0);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
template <typename T, typename C, typename A>
bool avl_tree<T,C,A>::rotate_left(typename avl_tree<T,C,A>::node_type * node)
  noexcept
{
	node_type * subtree_parent = node->parent_node();
	node_type * pivot = node->right;
	node_type * new_left = node;
	bool rc = true;

	( ( subtree_parent->left == node) ? subtree_parent->left :
	                                    subtree_parent->right ) = pivot;

	pivot->set_parent(subtree_parent);

	// move pivot's left subtree under right side of new_left
	new_left->right = pivot->left;
	if (new_left->right != nullptr)
		new_left->right->set_parent(new_left);

	// move new_right under right side of pivot
	pivot->left = new_left;
	new_left->set_parent(pivot);

	if (pivot->balance() > 0)
	{
		pivot->set_balance(0);
		new_left->set_balance(0);
	} else // if (pivot->balance() == 0)
	{
		pivot->set_balance(-1);
		new_left->set_balance(1);
		rc = false;
	}

	return rc;
}

//////////////////////////////////////////////////////////////////////
template <typename T, typename C, typename A>
bool avl_tree<T,C,A>::
double_rotate_left(typename avl_tree<T,C,A>::node_type * node)
  noexcept
{
	node_type * subtree_parent = node->parent_node();
	node_type * pivot = node->right->left;
	node_type * new_right = node->right;
	node_type * new_left = node;

	( ( subtree_parent->left == node) ? subtree_parent->left :
	                                    subtree_parent->right ) = pivot;

	pivot->set_parent(subtree_parent);

	new_right->left = pivot->right;
	if (new_right->left != nullptr)
		new_right->left->set_parent(new_right);

	new_left->right = pivot->left;
	if (new_left->right != nullptr)
		new_left->right->set_parent(new_left);

	pivot->left = new_left;
	new_left->set_parent(pivot);

	pivot->right = new_right;
	new_right->set_parent(pivot);

	if (pivot->balance() == -1)
	{
		new_left->set_balance(0);
		new_right->set_balance(1);
		pivot->set_balance(0);
	} else if (pivot->balance() == 0)
	{
		new_left->set_balance(0);
		new_right->set_balance(0);
	} else //if (pivot->balance == 1)
	{
		new_left->set_balance(-1);
		new_right->set_balance(0);
		pivot->set_balance(0);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
template <typename T, typename C, typename A>
void avl_tree<T,C,A>::
rebalance_after_insert(typename avl_tree<T,C,A>::node_type * n) noexcept
{
	node_type * last = n;
	for (node_type * current = n->parent_node(); current != &sentinel;
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
			if (current->right->balance() < 0)
				double_rotate_left(current);
			else
				rotate_left(current);
			break;
		} else if (tmp_balance < -1)
		{
			if (current->left->balance() > 0)
				double_rotate_right(current);
			else
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
	node_type * current = nullptr;
	node_type * parent = &sentinel;
	node_type ** child_link = &(sentinel.left);

	current = find_impl(n->value(), sentinel.left, parent, child_link);

	if (current != nullptr)
		return current;

	*child_link = n;

	n->set_parent(parent);

	if (parent == &sentinel)
		minimum = maximum = n;

	rebalance_after_insert(n);

	if (child_link == &(minimum->left))
		minimum = minimum->left;
	else if (child_link == &(maximum->right))
		maximum = maximum->right;

	++node_count;

	return n;
}

//////////////////////////////////////////////////////////////////////
template <typename T, typename C, typename A>
  std::pair<typename avl_tree<T,C,A>::node_type *, int>
  avl_tree<T,C,A>::
  delete_leaf_node(typename avl_tree<T,C,A>::node_type * target) noexcept
{
	node_type * parent = target->parent_node();
	int balance = 0;

	//printf("Deleting leaf\n");

	assert(  (  (parent == nullptr)
	         || (parent->left == target)
	         || (parent->right == target) )
          && (target->left == nullptr)
	      && (target->right == nullptr) );

	if (parent == &sentinel)
	{
		// deleting the tree root
		maximum = minimum = &sentinel;
		sentinel.left = nullptr;
	} else if (parent->left == target)
	{
		balance = parent->balance() + 1;

		parent->left = nullptr;

		if (target == minimum)
			minimum = parent;

	} else if (parent->right == target)
	{
		balance = parent->balance() - 1;

		parent->right = nullptr;

		if (target == maximum)
			maximum = parent;
	}

	return std::make_pair(parent, balance);
}

//////////////////////////////////////////////////////////////////////
template <typename T, typename C, typename A>
  std::pair<typename avl_tree<T,C,A>::node_type *, int>
  avl_tree<T,C,A>::
  delete_link_node(typename avl_tree<T,C,A>::node_type * target) noexcept
{
	node_type * parent = target->parent_node();
	int balance = 0;

	//printf("Deleting single link node\n");

	assert( (parent == nullptr)
	     || (parent->left == target)
	     || (parent->right == target) );

	assert((target->left == nullptr) ^ (target->right == nullptr));

	// only one child node is set, so we figure it out
	node_type * child = (target->left != nullptr) ?
	                    target->left :
	                    target->right;

	if (parent == &sentinel)
	{
		// deleting the tree root
		sentinel.left = child;
		child->set_parent(&sentinel);
	} else if (parent->left == target)
	{
		balance = parent->balance() + 1;
		parent->left = child;
		child->set_parent(parent);
	} else if (parent->right == target)
	{
		balance = parent->balance() - 1;
		parent->right = child;
		child->set_parent(parent);
	}

	if (target == minimum)
		minimum = leftmost_child(child);
	else if (target == maximum)
		maximum = rightmost_child(child);

	return std::make_pair(parent, balance);
}

//////////////////////////////////////////////////////////////////////
template <typename T, typename C, typename A>
  void avl_tree<T,C,A>::
  swap_with_neighbor(typename avl_tree<T,C,A>::node_type * target,
                     typename avl_tree<T,C,A>::node_type * neighbor) noexcept
{
	using std::swap;

	assert(target->left != nullptr && target->right != nullptr);
	assert(neighbor->left == nullptr || neighbor->right == nullptr);

	node_type * t_parent = target->parent_node();
	node_type * n_parent = neighbor->parent_node();

	if (t_parent->left == target)
		t_parent->left = neighbor;
	else // if (t_parent->right == target)
		t_parent->right = neighbor;

	if (n_parent->left == neighbor)
		n_parent->left = target;
	else // if (n_parent->right == neighbor)
		n_parent->right = target;

	swap(target->left, neighbor->left);
	swap(target->right, neighbor->right);
	target->set_parent(n_parent);
	neighbor->set_parent(t_parent);
	neighbor->left->set_parent(neighbor);
	neighbor->right->set_parent(neighbor);

	if (target->left != nullptr)
		target->left->set_parent(target);

	if (target->right != nullptr)
		target->right->set_parent(target);

	int tmpbalance = target->balance();
	target->set_balance(neighbor->balance());
	neighbor->set_balance(tmpbalance);
}

//////////////////////////////////////////////////////////////////////
template <typename T, typename C, typename A>
  std::pair<typename avl_tree<T,C,A>::node_type *, int>
  avl_tree<T,C,A>::
  delete_inner_node(typename avl_tree<T,C,A>::node_type * target) noexcept
{
	node_type * neighbor = nullptr;
	std::pair<node_type *, int> rc;

	//printf("Deleting inner node\n");
	assert(target->left != nullptr && target->right != nullptr);

	neighbor = (target->balance() < 0) ? rightmost_child(target->left)
	                                   : leftmost_child(target->right);

	swap_with_neighbor(target, neighbor);

	assert(target->left == nullptr || target->right == nullptr);

	if (target->left == nullptr && target->right == nullptr)
		rc = delete_leaf_node(target);
	else
		rc = delete_link_node(target);
		
	return rc;
}

//////////////////////////////////////////////////////////////////////
template <typename T, typename C, typename A>
  void avl_tree<T,C,A>::
  delete_node(typename avl_tree<T,C,A>::node_type * target) noexcept
{
	node_type * current = nullptr;
	int new_balance = 0;

	// TODO: Do we wish to make a more explicit runtime check here instead
	// of relying on this debug-only build statement?
	assert(target != &sentinel);

	// TODO: we make the determination of left vs right link node here...
	// we can avoid branching in delete_link_node() by calling
	// 2 versions of it here
	if (target->left == nullptr)
	{
		if (target->right == nullptr)
			std::tie(current, new_balance) = delete_leaf_node(target);
		else
			std::tie(current, new_balance) = delete_link_node(target);
	} else
	{
		if (target->right == nullptr)
			std::tie(current, new_balance) = delete_link_node(target);
		else
			std::tie(current, new_balance) = delete_inner_node(target);
	}

	node_type * last = current;
	while (current != &sentinel)
	{
		bool height_changed = false;

		if (new_balance < -1)
		{
			if (current->left->balance() == 1)
				height_changed = double_rotate_right(current);
			else
				height_changed = rotate_right(current);

		} else if (new_balance > 1)
		{
			if (current->right->balance() == -1)
				height_changed = double_rotate_left(current);
			else
				height_changed = rotate_left(current);

		} else if (new_balance == 0)
		{
			current->set_balance(new_balance);
			height_changed = true;
		} else // 1 or -1
		{
			current->set_balance(new_balance);
		}

		if ( ! height_changed )
			break;

		last = current;
		current = current->parent_node();
		if (current != nullptr)
		{
			new_balance = current->balance();
			if (current->left == last)
				++new_balance;
			else if (current->right == last)
				--new_balance;
		}

	}

	--node_count;
	destroy_node(target);
}

//////////////////////////////////////////////////////////////////////
template <typename T, typename Comp, typename Alloc>
void avl_tree<T, Comp, Alloc>::destroy_tree() noexcept
{
	node_type * p = sentinel.left;
	sentinel.left = nullptr;
	maximum = minimum = &sentinel;

	if (p != &sentinel)
	{

		if (p != nullptr) p->set_parent(nullptr);

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
	}

	assert(node_count == 0);
}

#endif // GUARD_AVL_TREE_H
