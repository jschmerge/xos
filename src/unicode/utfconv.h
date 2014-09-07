#ifndef GUARD_CODECVT_H
#define GUARD_CODECVT_H 1
#include <cwchar>
#include <locale>

enum codecvt_mode
{
	consume_header = 4,
	generate_header = 2,
	little_endian = 1
};
 
template<class Elem,
         unsigned long max_code = 0x10ffff,
         codecvt_mode Mode = (codecvt_mode)0>
class codecvt_utf8 : public std::codecvt<Elem, char, std::mbstate_t>
{
	static_assert(max_code <= 0x7fffffff,
	              "Max code must fit into a 31-bit integer");
 public:
	// suck this stuff in from codecvt
	using typename std::codecvt<Elem, char, std::mbstate_t>::state_type;
	using typename std::codecvt<Elem, char, std::mbstate_t>::extern_type;
	using typename std::codecvt<Elem, char, std::mbstate_t>::intern_type;
	using typename std::codecvt<Elem, char, std::mbstate_t>::result;

	explicit codecvt_utf8(std::size_t refs = 0)
	  : std::codecvt<Elem, char, std::mbstate_t>(refs) { }

	~codecvt_utf8() { }
 protected:
	bool do_always_noconv() const noexcept override
		{ return false; }

	int do_encoding() const noexcept override
		{ return 0; }

/*
	result do_in(state_type & state,
	             const extern_type * from,
	             const extern_type * from_end,
	             const extern_type * & from_next,
	             intern_type * to,
	             intern_type * to_limit,
	             intern_type * & to_nex) const override;

	int do_length(state_type & state,
	              const extern_type * from,
	              const extern_type * from_end,
	              std::size_t max) const override;
*/

	constexpr int do_max_length() const noexcept override
	{
		return ( (max_code <=       0x7f) ? 1 :
		         ( (max_code <=      0x7ff) ? 2 :
		           ( (max_code <=     0xffff) ? 3 :
		             ( (max_code <=   0x1fffff) ? 4 :
		               ( (max_code <=  0x3ffffff) ? 5 :
		                 ( (max_code <= 0x7fffffff) ? 6 : -1 ) ) ) ) ) );
	}

/*
	result do_out(state_type & state,
	              const intern_type * from,
	              const intern_type * from_end,
	              const intern_type * from_next,
	              extern_type * to,
	              extern_type * to_limit,
	              extern_type * & to_next) const override;

	result do_unshift(state_type & state,
	                  extern_type * to,
	                  extern_type * to_limit,
	                  extern_type * & to_next) const override;
*/
};

#if 0 
template<class Elem,
         unsigned long Maxcode = 0x10ffff,
         codecvt_mode Mode = (codecvt_mode)0>
class codecvt_utf16 : public std::codecvt<Elem, char, std::mbstate_t> {
 public:
    explicit codecvt_utf16(std::size_t refs = 0);
    ~codecvt_utf16();
};

template<class Elem,
         unsigned long Maxcode = 0x10ffff,
         codecvt_mode Mode = (codecvt_mode)0>
class codecvt_utf8_utf16 : public std::codecvt<Elem, char, std::mbstate_t> {
 public:
    explicit codecvt_utf8_utf16(std::size_t refs = 0);
    ~codecvt_utf8_utf16();
};
#endif

#endif // GUARD_CODECVT_H
