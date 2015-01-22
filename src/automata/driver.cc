#include "automaton.h"

enum class utf8 : unsigned { A, B, C, D, E, F, G, H, I, J, };

int main(int, char **)
{
	automaton<char, utf8> a;

	a.declare_start_state(utf8::A, true);
	a.declare_state(utf8::B);
	a.declare_state(utf8::C);
	a.declare_state(utf8::D);
	a.declare_state(utf8::E, true);
	a.declare_state(utf8::F);
	a.declare_state(utf8::G);
	a.declare_state(utf8::H);
	a.declare_state(utf8::I);
	a.declare_state(utf8::J);


	a.declare_transition(utf8::A, utf8::B, 0xef);


	a.dump_all();
	return 0;
}
