#ifndef GUARD_BULK_ALLOCATOR_H
#define GUARD_BULK_ALLOCATOR_H 1
#include <cstring>
#include <new>
#include <type_traits>
#include <vector>

template <typename T>
class homogenous_arena
{
 public:
	explicit homogenous_arena(std::size_t num_elem)
	  : use_map(num_elem * sizeof(T), false)
	  , data_start(static_cast<T*>(::operator new(num_elem * sizeof(T))))
	  , size(num_elem)
	  , offset(0)
		{ memset(data_start, 0, num_elem * sizeof(T)); }

	homogenous_arena(const homogenous_arena &) = delete;

	homogenous_arena & operator = (const homogenous_arena &) = delete;

	~homogenous_arena() { ::operator delete [] (data_start); }
	
	void * get()
	{
		if (offset >= size) throw std::bad_alloc();
		void * ret = data_start + offset;
		++offset;
		return ret;
	}

	void put(void *)
	{
	}

 private:
	std::vector<bool> use_map;
	T * data_start;
	const std::size_t size;
	std::size_t offset;
};

template <typename T>
class bulk_allocator
{
 public:
	typedef T value_type;
	typedef T * pointer;

	typedef T * const_pointer;
	typedef T & reference;
	typedef const T & const_reference;

	typedef std::true_type propagate_on_container_copy_assignment;
	typedef std::true_type propagate_on_container_move_assignment;
	typedef std::true_type propagate_on_container_swap;

	template< class U > struct rebind { typedef bulk_allocator<U> other; };

	explicit bulk_allocator(homogenous_arena<T> * a) : arena(a)
		{ }

	template <typename U>
	bulk_allocator(bulk_allocator<U> const & rhs)
	  : arena(reinterpret_cast< homogenous_arena<T> * >( rhs.arena))
		{ }

	pointer allocate(std::size_t)
	{
		return static_cast<pointer>(arena->get());
	}

	void deallocate(pointer p, std::size_t)
	{
		arena->put(p);
	}

	template <typename U>
	bool operator == (bulk_allocator<U> const & rhs) const
	{
		return arena == reinterpret_cast<const homogenous_arena<T> *>(rhs.arena);
	}

	template <typename U>
	bool operator!=(bulk_allocator<U> const & rhs) const
	{
		return !(*this == rhs);
	}

	void destroy(T * p)
	{
		p->~T();
	}

private:
	homogenous_arena<T> * arena;

	template<typename U> friend class bulk_allocator;
};

#endif // GUARD_BULK_ALLOCATOR_H
