#ifndef GUARD_AUTOMATON_H
#define GUARD_AUTOMATON_H 1

#include <limits>
#include <vector>
#include <set>
#include <cstdio>
#include <stdexcept>

//////////////////////////////////////////////////////////////////////
template <typename SID_T>
class state
{
 public:
	typedef SID_T state_id_type;

	state(const state_id_type & name, bool accept = false)
	  : m_id(name)
	  , m_accept(accept)
		{ }

	state(const state & other)
	  : m_id(other.m_id)
	  , m_accept(other.m_accept)
		{ }

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
	bool accept_state() const { return m_accept; }

 protected:
	state_id_type m_id;
	bool m_accept;
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
struct transition
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

	transition(const transition &) = default;

	transition & operator = (const transition &) = default;

	state_id_type m_old_state;
	state_id_type m_new_state;
	input_type m_value;
};

//////////////////////////////////////////////////////////////////////
// FIXME: this class really isn't a nice interface for this
// Represents the range [low,high]
template <typename VAL_T>
class value_range
{
 public:
	typedef VAL_T value_type;
	typedef std::size_t size_type;

	value_range()
	  : begin(0)
	  , end(0)
		{ }

	value_range(const value_type & low, const value_type & high)
	  : begin(low)
	  , end(high)
		{ }

	~value_range() { }

	bool contains(const value_type & v) const
		{ return ((v >= begin) && (v <= end)); }

	value_type min() const
		{ return begin; }
	value_type max() const
		{ return end; }

	size_type size() const
		{ return (end - begin); }

 private:
	value_type begin, end;
};

//////////////////////////////////////////////////////////////////////
template <typename IN_T, typename SID_T>
class automaton
{
 public:
	typedef IN_T input_type;
	typedef SID_T state_id_type;

	automaton()
	  : m_start_state(m_states.end())
		{ }

	automaton(const automaton & other) = default;

	automaton(automaton && other) = default;

	automaton & operator = (const automaton & other) = default;

	automaton & operator = (automaton && other) = default;

	~automaton() { }

	void declare_start_state(const state_id_type & id, bool accept = false)
	{
		auto i = m_states.emplace(id, accept).first;

		if (m_start_state != m_states.end())
			throw std::logic_error("Bad start state");

		m_start_state = i;
	}

	void declare_state(const state_id_type & id, bool accept = false)
	{
		m_states.emplace(id, accept);
	}

	void declare_transition(const state_id_type & old_state,
	                        const state_id_type & new_state,
	                        const input_type & value)
	{
		m_transitions.emplace_back(old_state, new_state, value);
	}

	void declare_transition_range(const state_id_type & old_state,
	                              const state_id_type & new_state,
	                              const value_range<input_type> & range)
	{
		for (input_type v = range.min(); v != range.max(); ++v)
			declare_transition(old_state, new_state, v);
	}

	int format_state(char buffer[], const state<state_id_type> & s)
	{
		const char * format = nullptr;
		if (m_start_state->id() == s.id() && ! s.accept_state())
			format = "<%02x>";
		else if (m_start_state->id() == s.id() && s.accept_state())
			format = "<<%02x>>";
		else if (s.accept_state())
			format = "((%02x))";
		else
			format = "(%02x)";

		return sprintf(buffer, format, s.id());
	}

	void dump_all(FILE * f = stdout)
	{
		fprintf(f, "%zu States:\n", m_states.size());
		for (auto s : m_states)
		{
			char buffer[16];
			format_state(buffer, s);
			fprintf(f, "\t%s\n", buffer);
		}

		fprintf(f, "%zd Transitions:\n", m_transitions.size());
		for (const auto & t : m_transitions)
		{
			if (m_start_state->id() == t.m_old_state)
				fprintf(f, "\t<%02x> --> ", t.m_old_state);
			else
				fprintf(f, "\t(%02x) --> ", t.m_old_state);

			if (m_start_state->id() == t.m_new_state)
				fprintf(f, "<%02x> [value = 0x%02hhx]\n", t.m_new_state,
				        t.m_value);
			else
				fprintf(f, "(%02x) [value = 0x%02hhx]\n", t.m_new_state,
				        t.m_value);
		}

	}

 protected:
	void compile() { }

 private:
	std::set<state<state_id_type>> m_states;
	typename std::set<state<state_id_type>>::iterator m_start_state;
	std::vector<transition<input_type, state_id_type>> m_transitions;
};

#endif // GUARD_AUTOMATON_H
