#ifndef GUARD_FS_PATH_H
#define GUARD_FS_PATH_H 1

#include <cstdint>
#include <string>
#include <vector>

namespace std {
 namespace experimental {
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

#if 0
		template <class Source>
		 path(const Source & source);
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
#if XXX
		// XXX - what is the runtime complexity here?
		path & operator /= (const path & p);
#endif

#if 0
		template <class Source>
		 path & operator /= (const Source & source);
		template <class Source>
		 path & append(const Source & source);
		template <class InputIterator>
		 path & append(InputIterator first, InputIterator last);
#endif

#if 0
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

		// modifiers
		void clear() noexcept;
		path & make_preferred();
		path & remove_filename();
		path & replace_filename(const path & replacement);
		path & replace_extension(const path & replacement = path());
		void swap(path & rhs) noexcept;

		// native format observers
		const string_type & native() const noexcept;
#endif
		const value_type * c_str() const noexcept;
#if 0
		operator string_type() const;

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

		// iterators
		class iterator;
		typedef iterator const_iterator;
		iterator begin() const;
		iterator end() const;
#endif

	 private:
		string_type pathname; // exposition only
		std::vector<string_type::size_type> seperators;
	};

   } /*v1*/
  }/*filesystem*/
 } /*experimental*/
} /*std*/


#endif // GUARD_FS_PATH_H
