#include <cwchar>
#include <cstdio>
#include <cstring>
#include <string>

size_t utf8strlen(const std::basic_string<char> & str)
{
	size_t sz = 0;
	size_t char_length = 0;
	for (std::basic_string<char>::const_iterator i = str.begin();
	     i != str.end(); i += char_length)
	{
	}
	return sz;
}

template <class C>
void stringInfo(const std::basic_string<C> & str)
{
	const char * p = reinterpret_cast<const char *>(str.c_str());

	printf("----------\nFunction = %s\ndata = %s\n", __PRETTY_FUNCTION__, p);
	printf("data.length() = %zd\n", str.length());
	printf("sizeof(C) * length = %zd\n", str.length() * sizeof(C));
	printf("strlen() of data = %zd\n", strlen(p));
	printf("wcslen() of data = %zd\n", wcslen(reinterpret_cast<const wchar_t*>(p)));

	for (size_t i = 0; i < ((str.length() + 1) * sizeof(C)); ++i)
		printf("%02hhx ", p[i]);

	putchar('\n');
}

int main()
{
	std::basic_string<char> simpleString = u8"foo bar";
	std::basic_string<wchar_t> simpleWString = L"foo bar";
	std::basic_string<char16_t> simpleU16String = u"foo bar";
	std::basic_string<char32_t> simpleU32String = U"foo bar";

	stringInfo(simpleString);
	stringInfo(simpleWString);
	stringInfo(simpleU16String);
	stringInfo(simpleU32String);

	std::basic_string<char> complexString = u8"外国語の学習と教授";
	std::basic_string<wchar_t> complexWString = L"外国語の学習と教授";
	std::basic_string<char16_t> complexU16String = u"外国語の学習と教授";
	std::basic_string<char32_t> complexU32String = U"外国語の学習と教授";

	stringInfo(complexString);
	stringInfo(complexWString);
	stringInfo(complexU16String);
	stringInfo(complexU32String);

	
	return 0;
}
