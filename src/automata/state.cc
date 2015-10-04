
#include <limits>
#include <cstdio>
#include <memory>
#include <set>
#include <string>
#include <map>
#include <vector>

#include "table.h"
#include "utfsample.h"

//////////////////////////////////////////////////////////////////////
template <typename IN_T>
struct transition {
	std::string from;
	typename std::make_unsigned<IN_T>::type begin;
	typename std::make_unsigned<IN_T>::type end;
};

//////////////////////////////////////////////////////////////////////
template <typename IN_T>
struct state_initializer {
	std::string name;
	std::initializer_list<transition<IN_T>> previous_states;
};

//////////////////////////////////////////////////////////////////////
template<typename IN_T = char>
class transition_function
{
 public:
	transition_function(std::initializer_list<IN_T> input_classes,
	                    std::vector<size_t> states);

	size_t operator () (const IN_T & in, size_t current_state);

 private:
	std::vector<IN_T> input_classes;
};

//////////////////////////////////////////////////////////////////////
template <typename IN_T = char>
class function_generator
{
 public:
	typedef typename std::make_unsigned<IN_T>::type input_type;
	typedef std::numeric_limits<input_type> input_limits;

 public:

	function_generator(input_type min = input_limits::min(),
	                   input_type max = input_limits::max())
	  : function_generator({}, min, max) { }


	function_generator(std::initializer_list<state_initializer<IN_T>> everything,
	                   input_type min = input_limits::min(),
	                   input_type max = input_limits::max())
	  : minimum_input_value(min)
	  , maximum_input_value(max)
	  , number_of_inputs(1ul + max - min)
	  , range_ends()
	  , states()
	  , state_names(everything.size() + 1)
	  , transition_table()
	{
		size_t state_number = 0;

		std::set<input_type> range_markers;
		range_markers.insert(maximum_input_value);
		for (auto state_init : everything)
		{
			state_names.at(state_number) = state_init.name;
			states[state_init.name] = state_number++;
			for (auto t : state_init.previous_states)
			{
				range_markers.insert(t.begin - 1);
				range_markers.insert(t.end);
			}
		}

		state_names.back() = "Error";
		states["Error"] = everything.size();

		range_ends.assign(range_markers.begin(), range_markers.end());

		transition_table = allocate_table();

		fill_table(everything);

		print_function();
	}

	~function_generator() { }

	void fill_table(
	      const std::initializer_list<state_initializer<IN_T>> & everything)
	{
		for (auto s_t : everything)
		{
			std::array<size_t, 2> index;
			size_t target = states[s_t.name];
			for (auto t : s_t.previous_states)
			{
				index[0] = states[t.from];
				for (size_t r_idx = 0; r_idx < range_ends.size(); ++r_idx)
				{
					if ( (range_ends[r_idx] >= t.begin)
					  && (range_ends[r_idx] <= t.end) )
					{
						index[1] = r_idx;
						transition_table->at(index) = target;
					}
				}
			}
		}
	}

	void print_function()
	{
		printf("Number of states: %zu\n", states.size());
		printf("Number of inputs: %zu\n", number_of_inputs);
		printf("Number of input classes: %zd\n", range_ends.size());

		printf("%20s %-2s  ", "", "");
		for (size_t x = 0; x < range_ends.size(); ++x)
			printf("%4zu", x);
		printf("\n%20s %-2s  ", "", "");
		for (auto r : range_ends)
			printf("%4x", r);
		printf("\n-------------------------------------"
		       "-------------------------------------\n");
		for (auto name : state_names)
		{
			printf("%20s:%-2zd |", name.c_str(), states[name]);
			for (size_t j = 0; j < range_ends.size(); ++j)
				printf("%4zu", transition_table->at({{states[name], j}}));
			printf("\n");
		}
	}

	void print_dot_function()
	{
		printf("digraph {\n"
		       "\trankdir=\"LR\"\n"
		       "\tStart [label=\"Start\",shape=point,"
		       "fontname=\"times bold italic\",fontsize=20]\n"
		       "Start -> \"%s\"", state_names[0].c_str());
		for (auto x : state_names)
		{
			printf("\t\"%s\" [shape=circle,"
			       "fontname=\"times bold italic\","
			       "fontsize=20]\n", x.c_str());
		}

		for (auto x : states)
		{
			input_type last = 0;
			for (size_t i = 0; i < range_ends.size(); ++i)
			{
				size_t to = transition_table->at(x.second, i);
				if (i == (range_ends.size() - 1))
				{
					if (to == 10)
					printf("\t\"%s\" -> \"%s\" [label=\"0x%02x-%02x\",color=\"red\"]\n",
					       x.first.c_str(),
					       state_names[to].c_str(), last, range_ends[i]);
					else
					printf("\t\"%s\" -> \"%s\" [label=\"0x%02x-%02x\"]\n",
					       x.first.c_str(),
					       state_names[to].c_str(), last, range_ends[i]);
				} else if (to != transition_table->at(x.second, i + 1))
				{
					if (to == 10)
					printf("\t\"%s\" -> \"%s\" [label=\"0x%02x-%02x\",color=\"red\"]\n",
					       x.first.c_str(),
					       state_names[to].c_str(), last, range_ends[i]);
					else
					printf("\t\"%s\" -> \"%s\" [label=\"0x%02x-%02x\"]\n",
					       x.first.c_str(),
					       state_names[to].c_str(), last, range_ends[i]);
					last = range_ends[i] + 1;
				}
			}
		}
		printf("}\n");
	}

	std::unique_ptr<lookup_table<size_t, 2>> allocate_table()
	{
		std::array<size_t, 2> table_dimensions{{states.size(),
		                                        range_ends.size()}};
		std::unique_ptr<lookup_table<size_t, 2>> ret{
			new lookup_table<size_t, 2>{table_dimensions, states.size() - 1}};

		return ret;
	}

	size_t operator () (size_t current_state, IN_T input)
	{
		size_t input_class = 0;
		auto class_iter = std::lower_bound(range_ends.begin(),
		                                   range_ends.end(),
		                                   static_cast<input_type>(input));
		input_class = std::distance(range_ends.begin(), class_iter);

		size_t ret =  (transition_table->at({{current_state, input_class}}));
		return ret;
	}

 protected:
	const input_type minimum_input_value;
	const input_type maximum_input_value;
	const size_t number_of_inputs;
	std::vector<input_type> range_ends;
	std::map<std::string, size_t> states;
	std::vector<std::string> state_names;
	std::unique_ptr<lookup_table<size_t, 2>> transition_table;
};


int main()
{
	function_generator<char> utf8_machine{ {
		{ "BOM Start", { } },
		{ "BOM 1", {
			{ "BOM Start", 0xef, 0xef }
		} },
		{ "BOM 2", {
			{ "BOM 1", 0xbb, 0xbb }
		} },
		{ "Complete BOM", {
			{ "BOM 2", 0xbf, 0xbf }
		} },
		{ "Complete Character", {
			{ "Complete Character", 0x0, 0x7f },
			{ "One Left", 0x80, 0xbf },
			{ "Complete BOM", 0x0, 0x7f },
			{ "BOM 2", 0x80, 0xbe },
			{ "BOM Start", 0x0, 0x7f },
		} },
		{ "One Left", {
			{ "Two Left", 0x80, 0xbf },
			{ "Complete Character", 0xc0, 0xdf },
			{ "Complete BOM", 0xc0, 0xdf },
			{ "BOM Start", 0xc0, 0xdf },
			{ "BOM 1", 0x80, 0xba },
			{ "BOM 1", 0xbc, 0xbf },
		} },
		{ "Two Left", {
			{ "BOM Start", 0xe0, 0xee },
			{ "Complete BOM", 0xe0, 0xef },
			{ "Complete Character", 0xe0, 0xef },
			{ "Three Left", 0x80, 0xbf },
		} },
		{ "Three Left", {
			{ "BOM Start", 0xf0, 0xf7 },
			{ "Complete BOM", 0xf0, 0xf7 },
			{ "Four Left", 0x80, 0xbf },
			{ "Complete Character", 0xf0, 0xf7 },
		} },
		{ "Four Left", {
			{ "Complete Character", 0xf8, 0xfb },
			{ "Complete BOM", 0xf8, 0xfb },
			{ "Five Left", 0x80, 0xbf },
			{ "BOM Start", 0xf8, 0xfb },
		} },
		{ "Five Left", {
			{ "Complete Character", 0xfc, 0xfd },
			{ "Complete BOM", 0xfc, 0xfd },
			{ "BOM Start", 0xfc, 0xfd },
		} },
	} };

//	const char * ptr = "\x01\x7f\x80\xba\xbb\xbc\xbe\xbf\xc0\xdf"
//	                   "\xe0\xee\xef\xf0\xf7\xf8\xfb\xfc\xfd\xfe\xff";

	const char * ptr = sample_utf8;

	size_t state = 0;

	while (*ptr)
	{
		state = utf8_machine(state, *ptr);
		++ptr;
	}

	utf8_machine.print_dot_function();
	return 0;
}
