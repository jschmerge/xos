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
         unsigned long Maxcode = 0x10ffff,
         codecvt_mode Mode = (codecvt_mode)0>
class codecvt_utf8 : public std::codecvt<Elem, char, std::mbstate_t>
{
 public:
	explicit codecvt_utf8(std::size_t refs = 0);
	~codecvt_utf8();
};

 
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

#endif // GUARD_CODECVT_H
