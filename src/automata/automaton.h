#ifndef GUARD_AUTOMATON_H
#define GUARD_AUTOMATON_H 1

#include <limits>
#include <vector>
#include <set>

//////////////////////////////////////////////////////////////////////
template <typename SID_T>
class state
{
 public:
	typedef SID_T state_id_type;

	state(const state_id_type & name) : m_id(name) { }

	state(const state & other) : m_id(other.m_id) { }

	state(state && other) noexcept : m_id(std::move(other.m_id)) { }

	state & operator = (const state & other)
	{
		if (this != &other)
			m_id = other.m_id;
		return *this;
	}

	state & operator = (state && other) noexcept
	{
		using std::swap;
		swap(m_id, other.m_id);
		return *this;
	}

	state_id_type id() const { return m_id; }

 protected:
	state_id_type m_id;
};

//////////////////////////////////////////////////////////////////////
template <typename SID_T>
bool operator == (const state<SID_T> & a, const state<SID_T> & b)
	{ return a.id() == b.id(); }

template <typename SID_T>
bool operator != (const state<SID_T> & a, const state<SID_T> & b)
	{ return a.id() != b.id(); }

template <typename SID_T>
bool operator < (const state<SID_T> & a, const state<SID_T> & b)
	{ return a.id() < b.id(); }

template <typename SID_T>
bool operator <= (const state<SID_T> & a, const state<SID_T> & b)
	{ return a.id() <= b.id(); }

template <typename SID_T>
bool operator > (const state<SID_T> & a, const state<SID_T> & b)
	{ return a.id() > b.id(); }

template <typename SID_T>
bool operator >= (const state<SID_T> & a, const state<SID_T> & b)
	{ return a.id() >= b.id(); }

//////////////////////////////////////////////////////////////////////
template <typename IN_T, typename SID_T>
class transition
{
 public:
	typedef IN_T input_type;
	typedef SID_T state_id_type;

	transition(const state_id_type & old_state,
	           const state_id_type & new_state,
	           const input_type & value)
	  : m_old_state(old_state)
	  , m_new_state(new_state)
	  , m_value(value)
		{ }

	transition(const transition &) = delete;

	transition & operator = (const transition &) = delete;

	transition(transition && other) noexcept
	  : m_old_state(std::move(other.m_old_state))
	  , m_new_state(std::move(other.m_new_state))
	  , m_value(std::move(other.m_value))
		{ }

	transition & operator = (transition && other) noexcept
	{
		using std::swap;
		swap(m_old_state, other.m_old_state);
		swap(m_new_state, other.m_new_state);
		swap(m_value, other.m_value);
		return *this;
	}

 private:
	state_id_type m_old_state;
	state_id_type m_new_state;
	input_type m_value;
};

//////////////////////////////////////////////////////////////////////
template <typename VAL_T>
class value_range
{
 public:
	typedef VAL_T value_type;
	typedef std::size_t size_type;

	value_range() { }

	~value_range() { }

	bool contains(const value_type & v) const;
	value_type min() const;
	value_type max() const;
	size_type size() const;
};

//////////////////////////////////////////////////////////////////////
template <typename IN_T, typename SID_T>
class automaton
{
 public:
	typedef IN_T input_type;
	typedef SID_T state_id_type;

	automaton() { }
	automaton(const automaton & other) = default;
	automaton(automaton && other) = default;
	automaton & operator = (const automaton & other) = default;
	automaton & operator = (automaton && other) = default;
	~automaton() { }

	void declare_start_state(const state_id_type & id)
	{
		declare_state(id);
	}

	void declare_state(const state_id_type & id)
	{
		m_states.emplace(id);
	}

	void declare_transition(const state_id_type & old_state,
	                        const state_id_type & new_state,
	                        const input_type & value)
	{
		m_transitions.emplace_back(old_state, new_state, value);
	}

#if 0
	void declare_transition_range(const state_id_type & old_state,
	                              const state_id_type & new_state,
	                              const input_range<input_type> & range)
	{
		for (input_type v = range.begin(); v != range.end(); ++v)
			m_transitions.emplace_back(old_state, new_state, v);
	}
#endif

 protected:
	void compile() { }

 private:
	std::set<state<state_id_type>> m_states;
	std::vector<transition<state_id_type, input_type>> m_transitions;
};

#endif // GUARD_AUTOMATON_H
