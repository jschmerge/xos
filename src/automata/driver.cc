#include "automaton.h"

enum class utf8 : unsigned { A, B, C, D, E, F, G, H, I, J, };

int main(int, char **)
{
	automaton<uint8_t, utf8> a;

	a.declare_start_state(utf8::A, true);
	a.declare_state(utf8::B);
	a.declare_state(utf8::C);
	a.declare_state(utf8::D, true);
	a.declare_state(utf8::E, true);
	a.declare_state(utf8::F);
	a.declare_state(utf8::G);
	a.declare_state(utf8::H);
	a.declare_state(utf8::I);
	a.declare_state(utf8::J);


	a.declare_transition_range(utf8::A, utf8::E, {0, 0x7f});
//	a.declare_transition_range(utf8::A, ERROR,   {0x80, 0xbf});
	a.declare_transition_range(utf8::A, utf8::F, {0xc0, 0xdf});
	a.declare_transition_range(utf8::A, utf8::J, {0xe0, 0xee});
	a.declare_transition(      utf8::A, utf8::B, 0xef);
	a.declare_transition_range(utf8::A, utf8::I, {0xf0, 0xf7});
	a.declare_transition_range(utf8::A, utf8::H, {0xf8, 0xfb});
	a.declare_transition_range(utf8::A, utf8::G, {0xfc, 0xfd});
//	a.declare_transition_range(utf8::A, ERROR,   {0xfe, 0xff});

//	a.declare_transition_range(utf8::B, ERROR,   {0x0, 0x7f});
	a.declare_transition_range(utf8::B, utf8::F, {0x80, 0xba});
	a.declare_transition(      utf8::B, utf8::C, 0xbb);
	a.declare_transition_range(utf8::B, utf8::F, {0xbc, 0xbf});
//	a.declare_transition_range(utf8::B, ERROR,   {0xc0, 0xff});

//	a.declare_transition_range(utf8::C, ERROR,   {0x0, 0x7f});
	a.declare_transition_range(utf8::C, utf8::E, {0x80, 0xbe});
	a.declare_transition(      utf8::C, utf8::D, 0xbf);
	a.declare_transition_range(utf8::C, utf8::F, {0xbc, 0xbf});
//	a.declare_transition_range(utf8::C, ERROR,   {0xc0, 0xff});

	a.declare_transition_range(utf8::D, utf8::E, {0, 0x7f});
//	a.declare_transition_range(utf8::D, ERROR,   {0x80, 0xbf});
	a.declare_transition_range(utf8::D, utf8::F, {0xc0, 0xdf});
	a.declare_transition_range(utf8::D, utf8::J, {0xe0, 0xef});
	a.declare_transition_range(utf8::D, utf8::I, {0xf0, 0xf7});
	a.declare_transition_range(utf8::D, utf8::H, {0xf8, 0xfb});
	a.declare_transition_range(utf8::D, utf8::G, {0xfc, 0xfd});
//	a.declare_transition_range(utf8::D, ERROR,   {0xfe, 0xff});

	a.declare_transition_range(utf8::E, utf8::E, {0, 0x7f});
//	a.declare_transition_range(utf8::E, ERROR,   {0x80, 0xbf});
	a.declare_transition_range(utf8::E, utf8::F, {0xc0, 0xdf});
	a.declare_transition_range(utf8::E, utf8::J, {0xe0, 0xef});
	a.declare_transition_range(utf8::E, utf8::I, {0xf0, 0xf7});
	a.declare_transition_range(utf8::E, utf8::H, {0xf8, 0xfb});
	a.declare_transition_range(utf8::E, utf8::G, {0xfc, 0xfd});
//	a.declare_transition_range(utf8::E, ERROR,   {0xfe, 0xff});


//	a.declare_transition_range(utf8::F, ERROR,   {0x0, 0x7f});
	a.declare_transition_range(utf8::F, utf8::E, {0x80, 0xbf});
//	a.declare_transition_range(utf8::F, ERROR,   {0xc0, 0xff});

//	a.declare_transition_range(utf8::G, ERROR,   {0x0, 0x7f});
	a.declare_transition_range(utf8::G, utf8::E, {0x80, 0xbf});
//	a.declare_transition_range(utf8::G, ERROR,   {0xc0, 0xff});

//	a.declare_transition_range(utf8::H, ERROR,   {0x0, 0x7f});
	a.declare_transition_range(utf8::H, utf8::I, {0x80, 0xbf});
//	a.declare_transition_range(utf8::H, ERROR,   {0xc0, 0xff});

//	a.declare_transition_range(utf8::I, ERROR,   {0x0, 0x7f});
	a.declare_transition_range(utf8::I, utf8::J, {0x80, 0xbf});
//	a.declare_transition_range(utf8::I, ERROR,   {0xc0, 0xff});

//	a.declare_transition_range(utf8::J, ERROR,   {0x0, 0x7f});
	a.declare_transition_range(utf8::J, utf8::F, {0x80, 0xbf});
//	a.declare_transition_range(utf8::J, ERROR,   {0xc0, 0xff});

	a.dump_all();
	return 0;
}
