#include "codecvt_specializations.h"

namespace std {

//////////////////////////////////////////////////////////////////////
//
// codecvt<char16_t, char, mbstate_t> member functions
//
//////////////////////////////////////////////////////////////////////

codecvt_base::result
codecvt<char16_t, char, mbstate_t>::do_out(
	mbstate_t & state,
	const char16_t * from_begin,
	const char16_t * from_end,
	const char16_t * & from_next,
	char * to_begin,
	char * to_end,
	char * & to_next) const
{
	namespace utf8 = utf8_conversion;
	namespace utf16 = utf16_conversion;

	result res = ok;

	from_next = from_begin;
	to_next = to_begin;

	while ((to_next < to_end) && (from_next < from_end) && (res == ok))
	{
		if (utf16::update_mbstate(state, *from_next))
			++from_next;
		else
			res = error;

		if ((state.__count == 0) && (res == ok))
		{
			// have a complete character
			char val = utf8::extract_leader_byte(state);
			if (state.__count < 0)
				res = error;

			*to_next = val;
			++to_next;
		}

		if (state.__count > 0)
		{
			res = do_unshift(state, to_next, to_end, to_next);
		}
	}

	return res;
}

codecvt_base::result
codecvt<char16_t, char, mbstate_t>::do_unshift(
	mbstate_t & state,
	char * to_begin,
	char * to_end,
	char * & to_next) const
{
	namespace utf8 = utf8_conversion;

	assert((state.__count) >= 0 && (state.__count < do_max_length()));

	to_next = to_begin;

	while ((to_next < to_end) && state.__count > 0)
	{
		*to_next = utf8::next_byte(state);
		++to_next;
		--state.__count;
	}

	return ( (state.__count == 0) ?  ok : partial );
}

codecvt_base::result
codecvt<char16_t, char, mbstate_t>::do_in(
	mbstate_t & state,
	const char * from_begin,
	const char * from_end,
	const char * & from_next,
	char16_t * to_begin,
	char16_t * to_end,
	char16_t * & to_next) const
{
	namespace utf8 = utf8_conversion;
	namespace utf16 = utf16_conversion;
	result res = ok;

	assert(from_begin <= from_end);
	assert(to_begin <= to_end);

	from_next = from_begin;
	to_next = to_begin;

	while ( (res == ok) && (from_next < from_end) && (to_next < to_end) )
	{
		if (state.__count < 0)
		{
			*to_next = state.__value.__wch;
			++to_next;
			state.__count = 0;
		}

		if (utf8::update_mbstate(state, *from_next))
		{
			++from_next;

			if (state.__count == 0)
			{
				*to_next = utf16::extract_leader_value(state);
				++to_next;
			}
		} else
		{
			res = error;
		}
	}

	if ( (res == ok) && (state.__count < 0) && (to_next < to_end))
	{
		*to_next = state.__value.__wch;
		++to_next;
		state.__count = 0;
	}

	return res;
}

int
codecvt<char16_t, char, mbstate_t>::do_length(
	mbstate_t & state,
	const char * from_begin,
	const char * from_end,
	size_t max) const
{
	namespace utf8 = utf8_conversion;

	size_t count = 0;
	const char * i = from_begin;

	while (  (i < from_end)
	      && (count < max)
	      && utf8::update_mbstate(state, *i))
	{
		if (state.__count == 0)
		{
			if (state.__value.__wch < 0x10000u)
			{
				++count;
			} else if (state.__value.__wch <= 0x10ffff)
			{
				count += 2;
			} else
			{
				break;
			}
			state.__value.__wch = 0;
		}
		++i;
	}

	return (i - from_begin);
}

//////////////////////////////////////////////////////////////////////
//
// codecvt<char32_t, char, mbstate_t> member functions
//
//////////////////////////////////////////////////////////////////////

codecvt_base::result
codecvt<char32_t, char, mbstate_t>::do_out(
	mbstate_t & state,
	const char32_t * from_begin,
	const char32_t * from_end,
	const char32_t * & from_next,
	char * to_begin,
	char * to_end,
	char * & to_next) const
{
	namespace utf8 = utf8_conversion;

	assert(from_begin <= from_end);
	assert(to_begin <= to_end);

	from_next = from_begin;
	to_next = to_begin;

	while ((to_next < to_end) && (from_next < from_end))
	{
		if (state.__count == 0)
		{
			state.__value.__wch = *from_next;
			char val = utf8::extract_leader_byte(state);

			if (state.__count < 0)
				return error;

			*to_next = val;
			++to_next;
			++from_next;
		}

		do_unshift(state, to_next, to_end, to_next);
	}

	do_unshift(state, to_next, to_end, to_next);

	return ( (  (state.__count == 0)
	         && (from_next == from_end)) ?
	         ok : partial );
}

codecvt_base::result
codecvt<char32_t, char, mbstate_t>::do_unshift(
	mbstate_t & state,
	char * to_begin,
	char * to_end,
	char * & to_next) const
{
	namespace utf8 = utf8_conversion;

	assert((state.__count) >= 0 && (state.__count < do_max_length()));

	to_next = to_begin;

	while ((to_next < to_end) && state.__count > 0)
	{
		*to_next = utf8::next_byte(state);
		++to_next;
		--state.__count;
	}

	return ( (state.__count == 0) ?  ok : partial );
}

codecvt_base::result
codecvt<char32_t, char, mbstate_t>::do_in(
	mbstate_t & state,
	const char * from_begin,
	const char * from_end,
	const char * & from_next,
	char32_t * to_begin,
	char32_t * to_end,
	char32_t * & to_next) const
{
	namespace utf8 = utf8_conversion;

	assert(from_begin <= from_end);
	assert(to_begin <= to_end);

	for (from_next = from_begin, to_next = to_begin;
	     (from_next < from_end) && (to_next < to_end);
	     ++from_next)
	{
		if ( ! utf8::update_mbstate(state, *from_next))
			return error;

		if (state.__count == 0)
		{
			*to_next = state.__value.__wch;
			++to_next;
		}
	}

	return (((state.__count != 0) || (from_next < from_end) ) ?
	        partial : ok );
}

int
codecvt<char32_t, char, mbstate_t>::do_length(
	mbstate_t & state,
	const char * from_begin,
	const char * from_end,
	size_t max) const
{
	namespace utf8 = utf8_conversion;

	size_t count = 0;
	const char * i = from_begin;

	while (  (i < from_end)
	      && (count < max)
	      && utf8::update_mbstate(state, *i))
	{
		if (state.__count == 0)
			++count;
		++i;
	}

	return (i - from_begin);
}

} // namespace std

