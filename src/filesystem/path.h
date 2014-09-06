#ifndef GUARD_FS_PATH_H
#define GUARD_FS_PATH_H 1

#include <cstdint>
#include <string>
#include <algorithm>
#include <vector>

#include "path_traits.h"

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

		~path();

		// assignments
		path & operator = (const path & p);
		path & operator = (path && p) noexcept;

		template <class Source>
		void dispatch_initialization(Source & src);

		template <class T>
		using char_encodable_t = path_traits::is_path_char_t_encodable<T>;

		template <class EcharT>
		enable_if_t<char_encodable_t<EcharT>::value>
		dispatch_initialization(const EcharT * src)
		{
			// FIXME
			pathname.assign(src);
		}

		template <class C, class T, class A>
		enable_if_t<char_encodable_t<C>::value>
		dispatch_initialization(const std::basic_string<C, T, A> &  src)
		{
			// FIXME
			pathname.assign(src);
		}

		template <class Source> path(const Source & src)
		{
			dispatch_initialization(src);
		}



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
		template <class EcharT,
				  class traits = std::char_traits<EcharT>,
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
		string_type pathname;
	};

} /*v1*/
}/*filesystem*/


#endif // GUARD_FS_PATH_H
