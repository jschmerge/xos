#ifndef GUARD_SPINLOCK_H
#define GUARD_SPINLOCK_H 1

#include <atomic>

class SpinLock
{
 public:
	SpinLock() : spinFlag{ATOMIC_FLAG_INIT} { }

	SpinLock(const SpinLock &) = delete;
	SpinLock & operator = (const SpinLock &) = delete;

	// Is this correct?
	~SpinLock() { lock(); }

	void lock()
	{
		while (spinFlag.test_and_set(std::memory_order_acquire))
		{
			asm("pause" : : :);
		}
	}
	
	bool try_lock()
		{ return (! spinFlag.test_and_set(std::memory_order_acquire)); }

	void unlock() { spinFlag.clear(std::memory_order_release); }

 private:
	std::atomic_flag spinFlag;
};

#endif // GUARD_SPINLOCK_H
