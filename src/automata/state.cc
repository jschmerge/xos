
#include <limits>
#include <cstdio>
#include <set>
#include <string>
#include <vector>

template <typename IN_T>
struct transition {
	std::string from;
	IN_T begin;
	IN_T end;
};

template <typename IN_T>
struct s_and_t {
	std::string s;
	std::initializer_list<transition<IN_T>> t;
};

template <typename IN_T = char>
class transition_function
{
 public:
	typedef IN_T input_type;
	typedef std::numeric_limits<input_type> input_limits;

	transition_function(input_type min = input_limits::min(),
	                    input_type max = input_limits::max())
	  : transition_function({}, min, max) { }


	transition_function(std::initializer_list<s_and_t<IN_T>> everything,
	                    input_type min = input_limits::min(),
	                    input_type max = input_limits::max())
	  : minimum_input_value(min)
	  , maximum_input_value(max)
	  , number_of_inputs(1ul + max - min)
	  , states()
	{
		std::set<input_type> range_markers;
		printf("Number of inputs: %zu\n", number_of_inputs);
		printf("States:\n");
		for (auto s_t : everything)
		{
			printf("\t%s\n", s_t.s.c_str());
			states.push_back(s_t.s);
			for (auto t : s_t.t)
			{
				range_markers.insert(t.begin);
				range_markers.insert(t.end + 1);
			}
		}

		for (auto x : range_markers)
			printf("%02hhx\n", static_cast<uint8_t>(x));
		printf("----\n");
		int s = 0;
		for (auto i = range_markers.begin(); i != range_markers.end(); ++i, ++s)
		{
			auto e = i;
			++e;
			if (e == range_markers.end())
				e = range_markers.begin();

			printf("%d: %02hhx-%02hhx\n", s,
			       static_cast<uint8_t>(*i),
			       static_cast<uint8_t>(*e - 1));
		}
	}


	~transition_function() { }

 protected:
	const input_type minimum_input_value;
	const input_type maximum_input_value;
	const size_t number_of_inputs;
	std::vector<std::string> states;
};


int main()
{
	transition_function<unsigned char> utf8_machine{ {
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

	return 0;
}
