#ifndef GUARD_SM_H
#define GUARD_SM_H 1

#include <cstddef>
#include <vector>
#include <memory>

template <typename STATE_T, typename INPUT_T = uint8_t>
class state_machine
{
 public:
	typedef INPUT_T input_type;
	typedef STATE_T state_type;
	typedef void (*state_change_function)(const state_type &,
	                                      const state_type &,
	                                      const input_type &);

	struct sm_item
	{
		sm_item(state_type old_s, state_type new_s, size_t input_class,
		        state_change_function f = nullptr) 
		  : old_state(old_s) , input(input_class)
		  , new_state(new_s) , callback(f) { }

		state_type old_state;
		size_t input;
		state_type new_state;
		state_change_function callback;
	};

	state_machine(size_t num_states, size_t num_input_classes,
	              std::initializer_list<sm_item> init_data,
	              std::initializer_list<input_type> classes)
	  : transition_table(num_states * num_input_classes, 0)
	  , function_table(num_states * num_input_classes, nullptr)
	  , input_class_mapping(classes)
	{
		for (const auto & d : init_data)
		{
			printf("%zd %zd -> %zd\n", d.old_state, d.input, d.new_state);
			transition_table[index_of(d.old_state, d.input)] = d.new_state;
			function_table[index_of(d.old_state, d.input)] = d.callback;
		}

		for (size_t i = 0; i < num_states; ++i)
		{
			for (size_t j = 0; j < num_input_classes; ++j)
			{
				printf("%hhx ", transition_table[index_of(i, j)]);
			}
			printf("\n");
		}
	}

	state_machine(const state_machine & other);
	state_machine(state_machine && other);

	~state_machine() { }

	state_machine & operator = (const state_machine & other);
	state_machine & operator = (state_machine && other);

	bool consume(const input_type * buffer, size_t length);

 protected:
	size_t index_of(state_type s, size_t i)
	{
		return ((s * input_class_mapping.size()) + i);
	}

	state_type current_state;

	std::vector<state_type> transition_table;
	std::vector<state_change_function> function_table;
	std::vector<input_type> input_class_mapping;
};

#endif // GUARD_SM_H
