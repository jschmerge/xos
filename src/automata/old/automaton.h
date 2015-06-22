#ifndef GUARD_AUTOMATON_H
#define GUARD_AUTOMATON_H 1

#include <limits>
#include <vector>
#include <list>
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

	~value_range() = default;

	value_type min() const
		{ return begin; }

	value_type max() const
		{ return end; }

	size_type size() const
		{ return (end - begin); }

	bool intersects(const value_range & other)
	{
		return (  ( other.begin >= begin && other.begin <= end )
		       || ( other.end >= begin   && other.end <= end )
		       || ( begin >= other.begin && begin <= other.end )
		       || ( end >= other.begin   && end <= other.end ) );
	}

	bool operator == (const value_range & other)
	{ return ( (begin == other.begin) && (end == other.end) ); }

 private:
	value_type begin, end;
};

//////////////////////////////////////////////////////////////////////
template <typename T>
std::vector<value_range<T>>
split_ranges(const value_range<T> & a, const value_range<T> & b)
{
	std::vector<value_range<T>> ret;
	ret.reserve(3);
	if (a.min() < b.min())
	{
		ret.emplace_back(a.min(), b.min() - 1);

		if (a.max() < b.max())
		{
			ret.emplace_back(b.min(), a.max());
			ret.emplace_back(a.max() + 1, b.max());
		} else if (b.max() < a.max())
		{
			ret.emplace_back(b.min(), b.max());
			ret.emplace_back(b.max() + 1, a.max());
		} else if (b.max() == a.max())
		{
			ret.emplace_back(b.min(), a.max());
		}

	} else if (a.min() > b.min())
	{
		ret.emplace_back(b.min(), a.min() - 1);

		if (b.max() < a.max())
		{
			ret.emplace_back(a.min(), b.max());
			ret.emplace_back(b.max() + 1, a.max());
		} else if (a.max() < b.max())
		{
			ret.emplace_back(a.min(), a.max());
			ret.emplace_back(a.max() + 1, b.max());
		} else if (b.max() == a.max())
		{
			ret.emplace_back(a.min(), a.max());
		}

	} else // a.min == b.min
	{
		if (a.max() < b.max())
		{
			ret.emplace_back(a.min(), a.max());
			ret.emplace_back(a.max() + 1, b.max());
		} else if (b.max() < a.max())
		{
			ret.emplace_back(a.min(), b.max());
			ret.emplace_back(b.max() + 1, a.max());
		} else
		{
			ret.emplace_back(a.min(), a.max());
		}
	}
	return ret;
}

//////////////////////////////////////////////////////////////////////
template <typename IN_T, typename SID_T>
struct transition
{
 public:
	typedef IN_T input_type;
	typedef value_range<IN_T> input_range_type;
	typedef SID_T state_id_type;

	transition(const state_id_type & old_state,
	           const state_id_type & new_state,
	           const input_type & value)
	  : m_old_state(old_state)
	  , m_new_state(new_state)
	  , m_range(value, value)
		{ }

	transition(const state_id_type & old_state,
	           const state_id_type & new_state,
	           const input_range_type & range)
	  : m_old_state(old_state)
	  , m_new_state(new_state)
	  , m_range(range)
		{ }

	transition(const transition &) = default;
	transition(transition &&) = default;
	transition & operator = (const transition &) = default;
	transition & operator = (transition &&) = default;

	state_id_type m_old_state;
	state_id_type m_new_state;
	input_range_type m_range;
};

//////////////////////////////////////////////////////////////////////
template <typename IN_T, typename SID_T>
class automaton
{
 public:
	typedef IN_T input_type;
	typedef SID_T state_id_type;

	automaton() : m_start_state(m_states.end()) { }

	automaton(const automaton & other) = default;
	automaton(automaton && other) = default;
	automaton & operator = (const automaton & other) = default;
	automaton & operator = (automaton && other) = default;
	~automaton() = default;

	void declare_start_state(const state_id_type & id, bool accept = false)
	{
		auto i = m_states.emplace(id, accept).first;

		if (m_start_state != m_states.end())
			throw std::logic_error("Bad start state");

		m_start_state = i;
	}

	void declare_state(const state_id_type & id, bool accept = false)
		{ m_states.emplace(id, accept); }

	void add_input_range(const value_range<input_type> & r)
	{
		std::vector<
			typename std::list<value_range<input_type>>::iterator> removals;

		for (auto i = input_boundaries.begin();
		     i != input_boundaries.end(); ++i)
		{
			if (*i == r)
			{
				return;
			}
		}

		std::vector<value_range<input_type>> nr;
		for (auto i = input_boundaries.begin();
		     i != input_boundaries.end(); ++i)
		{
			if (i->intersects(r))
			{
				auto x = split_ranges(*i, r);

				nr.insert(nr.end(), x.begin(), x.end());

				removals.push_back(i);
			}
		}

		if (nr.empty())
			input_boundaries.emplace_back(r);

		for (auto iter : removals)
			input_boundaries.erase(iter);

		for (auto y : nr)
			add_input_range(y);
	}

	void declare_transition(const state_id_type & old_state,
	                        const state_id_type & new_state,
	                        const input_type & value)
	{
		add_input_range(value_range<input_type>(value, value));
		m_transitions.emplace_back(old_state, new_state, value);
	}

	void declare_transition(const state_id_type & old_state,
	                        const state_id_type & new_state,
	                        const value_range<input_type> & range)
	{
		add_input_range(range);
		m_transitions.emplace_back(old_state, new_state, range);
	}

	int format_state(char buffer[], const state<state_id_type> & s)
	{
		const char * format = nullptr;
		if (m_start_state->id() == s.id() && ! s.accept_state())
			format = " <%02x> ";
		else if (m_start_state->id() == s.id() && s.accept_state())
			format = "<<%02x>>";
		else if (s.accept_state())
			format = "((%02x))";
		else
			format = " (%02x) ";

		return sprintf(buffer, format, s.id());
	}

	void dump_all(FILE * f = stdout)
	{
		char buffer1[16];
		char buffer2[16];

		fprintf(f, "%zu States:\n", m_states.size());
		for (auto s : m_states)
		{
			format_state(buffer1, s);
			fprintf(f, "\t%s\n", buffer1);
		}

		input_boundaries.sort([](const value_range<input_type> & a,
		                         const value_range<input_type> & b) {
			return (a.min() < b.min());
		});

		printf("%zu Input classes:\n", input_boundaries.size());
		int x = 0;
		for (auto r : input_boundaries)
		{
			printf("\t[%02x - %02x] - > %d\n", r.min(), r.max(), x++);
		}

		fprintf(f, "%zd Transitions:\n", m_transitions.size());
		for (const auto & t : m_transitions)
		{
			auto o = m_states.find(t.m_old_state);
			auto n = m_states.find(t.m_new_state);

			if (o == m_states.end() || n == m_states.end())
				throw std::logic_error("Bad state");

			format_state(buffer1, *o);
			format_state(buffer2, *n);

			fprintf(f, "%s --> %s [value = 0x%02x-%02x]\n", buffer1, buffer2,
			        t.m_range.min(), t.m_range.max());
		}
	}

 protected:
	void compile() { }

 private:
	std::set<state<state_id_type>> m_states;
	typename std::set<state<state_id_type>>::iterator m_start_state;
	std::vector<transition<input_type, state_id_type>> m_transitions;
	std::list<value_range<input_type>> input_boundaries;
};

#endif // GUARD_AUTOMATON_H
