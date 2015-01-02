#ifndef GUARD_AUTOMATA_H
#define GUARD_AUTOMATA_H 1

#include <string>
#include <limits>
#include <vector>

//////////////////////////////////////////////////////////////////////
template <typename S_ID_T>
class state
{
 public:
	typedef S_ID_T state_id_type;

	state(const std::string & name) : m_id(name) { }

	state(const state & other) : m_id(other.m_id) { }

	state & operator = (const state & other)
	{
		if (this != &other)
			m_id = other.m_id;
		return *this;
	}

	state(state && other) noexcept : m_id(std::move(other.m_id)) { }

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
template <typename IN_T, typename S_ID_T>
class transition
{
 public:
	typedef IN_T input_type;
	typedef S_ID_T state_id_type;

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
template <typename IN_T, typename S_ID_T>
class automaton
{
 public:
	typedef IN_T input_type;
	typedef S_ID_T state_id_type;

	automaton() { }
	automaton(const automaton & other) = default;
	automaton(automaton && other) = default;
	automaton & operator = (const automaton & other) = default;
	automaton & operator = (automaton && other) = default;
	~automaton() { }

	void declare_start_state(const state_id_type & id)
	{
		using std::swap;
		declare_state(id);
		swap(m_states.back(), m_states.front());
	}

	void declare_state(const state_id_type & id)
	{ m_states.emplace_back(id); }

	void declare_transition(const state_id_type & old_state,
	                        const state_id_type & new_state,
	                        const input_type & value)
	{ m_transitions.emplace_back(old_state, new_state, value); }

 protected:
	void compile() { }

 private:
	std::vector<state<state_id_type>> m_states;
	std::vector<transition<state_id_type, input_type>> m_transitions;
};

#endif // GUARD_AUTOMATA_H
