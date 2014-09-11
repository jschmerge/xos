#ifndef GUARD_FS_PATH_H
#define GUARD_FS_PATH_H 1

#include <cstdint>
#include <string>
#include <cstring>
#include <algorithm>
#include <vector>

#include "path_traits.h"
#include "../unicode/utfconv.h"

namespace filesystem {
inline namespace v1 {

class path
{
 public:
	typedef char value_type;
	typedef std::basic_string<value_type> string_type;
	static constexpr value_type preferred_separator = '/';

	// constructors and destructor
	path() noexcept;
	path(const path & p);
	path(path && p) noexcept;

	template <class Source>
	  path(const Source & src)
		{ dispatch_initialization(src); }

	~path();

	// assignments
	path & operator = (const path & p);
	path & operator = (path && p) noexcept;

	template <class Source>
	void dispatch_initialization(Source & src);

	template <class T>
	using char_encodable_t = path_traits::is_path_char_t_encodable<T>;

#if 0
	template <class InputIterator>
	 path(InputIterator first, InputIterator last);
	template <class Source>
	 path(const Source & source, const std::locale & loc);
	template <class InputIterator>
	 path(InputIterator first, InputIterator last, const std::locale & loc);

	template <class Source>
	 path & operator = (const Source & source);
	template <class Source>
	 path & assign(const Source & source);
	template <class InputIterator>
	 path & assign(InputIterator first, InputIterator last);

#endif
	// appends
	path & operator /= (const path & p);
#if 0

	template <class Source>
	 path & operator /= (const Source & source);
	template <class Source>
	 path & append(const Source & source);
	template <class InputIterator>
	 path & append(InputIterator first, InputIterator last);
#endif

	// concatenation
	path & operator += (const path & other);
	path & operator += (const string_type & other);
	path & operator += (const value_type * other);
	path & operator += (value_type other);

#if 0
	template <class Source>
	 path & operator += (const Source & x);
	template <class EcharT>
	 path & operator += (EcharT x);
	template <class Source>
	 path & concat(const Source & x);
	template <class InputIterator>
	 path & concat(InputIterator first, InputIterator last);

#endif
	// modifiers
	void clear() noexcept;
	path & make_preferred();
#if 0
	path & remove_filename();
	path & replace_filename(const path & replacement);
	path & replace_extension(const path & replacement = path());
#endif
	void swap(path & rhs) noexcept;
	// native format observers
	const string_type & native() const noexcept;
	const value_type * c_str() const noexcept;
	operator string_type() const;
#if 0

	template <class EcharT,
			  class traits = std::char_traits<EcharT>,
			  class Allocator = std::allocator<EcharT> >
	 std::basic_string<EcharT, traits, Allocator>
	  string(const Allocator & a = Allocator()) const;

	std::string string() const;
	std::wstring wstring() const;
	std::string u8string() const;
	std::u16string u16string() const;
	std::u32string u32string() const;

	// generic format observers
	template <class EcharT, class traits = std::char_traits<EcharT>,
			  class Allocator = std::allocator<EcharT> >
	 std::basic_string<EcharT, traits, Allocator>
	  generic_string(const Allocator & a = Allocator()) const;
	std::string generic_string() const;
	std::wstring generic_wstring() const;
	std::string generic_u8string() const;
	std::u16string generic_u16string() const;
	std::u32string generic_u32string() const;

	// compare
#endif
	int compare(const path & p) const noexcept;
	int compare(const string_type & s) const;
	int compare(const value_type * s) const;
#if 0

	// decomposition
	path root_name() const;
	path root_directory() const;
	path root_path() const;
	path relative_path() const;
	path parent_path() const;
	path filename() const;
	path stem() const;
	path extension() const;

#endif
	// query
	bool empty() const noexcept;
	bool has_root_name() const;
	bool has_root_directory() const;
	bool has_root_path() const;
	bool has_relative_path() const;
	bool has_parent_path() const;
	bool has_filename() const;
	bool has_stem() const;
	bool has_extension() const;
	bool is_absolute() const;
	bool is_relative() const;
#if 0

	// iterators
	class iterator;
	typedef iterator const_iterator;
	iterator begin() const;
	iterator end() const;
#endif

 private:

	template <class ECharT>
	enable_if_t<char_encodable_t<ECharT>::value>
	dispatch_initialization(const ECharT * src)
	{
		using result = std::codecvt_base::result;
		size_t len = 0;
		codecvt_utf8<ECharT> cvt;

		for (len = 0; src[len] != 0; ++len) { }

		pathname.resize((len * cvt.max_length()) + 1, '\0');
		if ( ! cvt.always_noconv())
		{

			const ECharT * from_next = nullptr;
			char * to_next = nullptr;
			std::mbstate_t mbs = std::mbstate_t();
			result r = cvt.out(mbs, src, src + len, from_next,
					           const_cast<char*>(pathname.data()),
					           const_cast<char*>(pathname.data()
			                   + pathname.length()), to_next);

			if (r != std::codecvt_base::ok)
				throw std::runtime_error(
				        "Could not convert pathname encoding");

			pathname.erase(to_next - pathname.data());
		} else
		{
			memcpy(const_cast<char*>(pathname.data()), src, len);
			pathname.resize(len);
		}
	}

	template <class ECharT, class T, class A>
	enable_if_t<char_encodable_t<ECharT>::value>
	dispatch_initialization(const std::basic_string<ECharT, T, A> & src)
	{
		using result = std::codecvt_base::result;
		codecvt_utf8<ECharT> cvt;

		pathname.resize((src.length() * cvt.max_length()) + 1, '\0');
		if ( ! cvt.always_noconv())
		{

			const ECharT * from_next = nullptr;
			char * to_next = nullptr;
			std::mbstate_t mbs = std::mbstate_t();
			result r = cvt.out(mbs, src.data(), src.data() + src.length(),
			                   from_next, const_cast<char*>(pathname.data()),
					           const_cast<char*>(pathname.data()
			                   + pathname.length()), to_next);

			if (r != std::codecvt_base::ok)
				throw std::runtime_error(
				        "Could not convert pathname encoding");

			pathname.erase(to_next - pathname.data());
		} else
		{
			memcpy(const_cast<char*>(pathname.data()),
			       src.data(), src.length());
			pathname.resize(src.length());
		}
	}

	string_type pathname;
};

} /*v1*/
}/*filesystem*/

#endif // GUARD_FS_PATH_H
