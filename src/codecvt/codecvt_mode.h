#ifndef GUARD_CODECVT_MODE_H
#define GUARD_CODECVT_MODE_H 1


namespace std {

constexpr char32_t max_unicode_codepoint()
{ return 0x10ffffu; }

//
// enum codecvt_mode
//
// - If (Mode & consume_header), the facet shall consume an initial header
//   sequence, if present, when reading a multibyte sequence to determine
//   the endianness of the subsequent multibyte sequence to be read.
// - If (Mode & generate_header), the facet shall generate an initial header
//   sequence when writing a multibyte sequence to advertise the endianness
//   of the subsequent multibyte sequence to be written.
// - If (Mode & little_endian), the facet shall generate a multibyte
//   sequence in little-endian order, as opposed to the default big-endian
//   order.
//
enum codecvt_mode
{
	consume_header = 4,
	generate_header = 2,
	little_endian = 1
};

} // namespace std

#endif // GUARD_CODECVT_MODE_H
