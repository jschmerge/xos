#ifndef GUARD_FS_PATH_H
#define GUARD_FS_PATH_H 1

#include <cstdint>
#include <string>
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

		template <class Source,
			      typename std::enable_if<
		              path_traits::is_path_initializer<
		                  Source>::is_ntcts_terminated,
			          Source*>::type = nullptr>
		path(const Source & src);

		template <class Source,
			      typename std::enable_if<
		              ! path_traits::is_path_initializer<
		                  Source>::is_ntcts_terminated,
		              Source*>::type = nullptr>
		path(const Source & src);

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

		// concatenation
		path & operator += (const path & x);
		path & operator += (const string_type & x);
		path & operator += (const value_type * x);
		path & operator += (value_type x);

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
		int compare(const path & p) const noexcept;
		int compare(const string_type & s) const;
		int compare(const value_type * s) const;

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
#if 0
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

		// iterators
		class iterator;
		typedef iterator const_iterator;
		iterator begin() const;
		iterator end() const;
#endif

	 private:
		string_type pathname; // exposition only
	};

#if 0
	template <class Source,
		      typename std::enable_if<
	              path_traits::is_path_initializer<
	                  Source>::is_ntcts_terminated,
		          Source*>::type>
	path::path(const Source & src)
	{
		for (auto i = src;
		     *i != path_traits::is_path_initializer<Source>::eos; ++i)
		{
			pathname.push_back(*i);
		}
	}

	template <class Source,
		      typename std::enable_if<
	              ! path_traits::is_path_initializer<
	                  Source>::is_ntcts_terminated,
	              Source*>::type>
	path::path(const Source & src)
	{
		pathname.reserve(src.size());
		for (const auto & i : src)
		{
			pathname.push_back(i);
		}
	}
#endif

} /*v1*/
}/*filesystem*/


#endif // GUARD_FS_PATH_H
