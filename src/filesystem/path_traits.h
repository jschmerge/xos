#ifndef GUARD_PATH_TRAITS_H
#define GUARD_PATH_TRAITS_H 1

#include <string>
#include <vector>
#include <deque>
#include <list>

namespace path_traits {

// For now, these are just a dump copy of boost::fs's traits...
// The way boost specifies these is pretty dumb

template <class T>
struct is_convertable { static constexpr bool value = false; };

template <> struct is_convertable<char*>
	{ static constexpr bool value = true; };

template <> struct is_convertable<const char*>
	{ static constexpr bool value = true; };

template <> struct is_convertable<wchar_t*>
	{ static constexpr bool value = true; };

template <> struct is_convertable<const wchar_t*>
	{ static constexpr bool value = true; };

template <> struct is_convertable<std::string>
	{ static constexpr bool value = true; };

template <> struct is_convertable<std::wstring>
	{ static constexpr bool value = true; };

template <> struct is_convertable<std::vector<char>>
	{ static constexpr bool value = true; };

template <> struct is_convertable<std::vector<wchar_t>>
	{ static constexpr bool value = true; };

template <> struct is_convertable<std::deque<char>>
	{ static constexpr bool value = true; };

template <> struct is_convertable<std::deque<wchar_t>>
	{ static constexpr bool value = true; };

template <> struct is_convertable<std::list<char>>
	{ static constexpr bool value = true; };

template <> struct is_convertable<std::list<wchar_t>>
	{ static constexpr bool value = true; };

} // namespace path_traits

#endif // GUARD_PATH_TRAITS_H
