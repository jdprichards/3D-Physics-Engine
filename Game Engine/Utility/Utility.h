//================================================
// Miscellaneous Utility Functions and Helpers
//================================================

#ifndef _UTILITY_H_INCLUDED_
#define _UTILITY_H_INCLUDED_

#include <string>
#include <type_traits>

//=============
// Marcos
//=============

// Marco to provide bitwise operators to use a given enum's value as flags - see how ImportFlags is used in Mesh.cpp for example of use
// Must incude <type_traits>. Cannot use for enums declared inside a class
#define ENUM_FLAG_OPERATORS(E) \
using E_INT = std::underlying_type_t<E>; \
inline E  operator& (E a, E b)   { return static_cast<E>(static_cast<E_INT>(a) & static_cast<E_INT>(b)); } \
inline E  operator| (E a, E b)   { return static_cast<E>(static_cast<E_INT>(a) | static_cast<E_INT>(b)); } \
inline E  operator^ (E a, E b)   { return static_cast<E>(static_cast<E_INT>(a) ^ static_cast<E_INT>(b)); } \
inline E  operator~ (E a)        { return static_cast<E>(~static_cast<E_INT>(a)); } \
inline E& operator&=(E& a, E b)  { a = a & b; return a; } \
inline E& operator|=(E& a, E b)  { a = a | b; return a; } \
inline E& operator^=(E& a, E b)  { a = a ^ b; return a; } \
inline bool IsSet(E a) { return static_cast<E_INT>(a) != 0; }


//===========================
// String Helper Functions
//===========================
// Functions to check if a string begins or ends with a given substring
// Case insensitvie versions an denoted "CI"

// Returns ture if string begins with substr, case insensitive
inline bool StartsWith(const std::string& str, const std::string& substr)
{
	if (substr.size() > str.size())
		return false;
	return std::equal(substr.begin(), substr.end(), str.begin());
}

// Returns true if str ends with substr
inline bool EndsWith(const std::string& str, const std::string& substr)
{
	if (substr.size() > str.size())
		return false;
	return std::equal(substr.rbegin(), substr.rend(), str.rbegin());
}

// Returns true if str begins with substr, case insensitive
inline bool StartsWithCI(const std::string& str, const std::string& substr)
{
	if (substr.size() > str.size())
		return false;
	return std::equal(substr.begin(), substr.end(), str.begin(), [](unsigned char a, unsigned char b) {return std::tolower(a) == std::tolower(b); });
}

// Returns true if str ends with substr, case insensitive
inline bool EndsWithCI(const std::string& str, const std::string& substr)
{
	if (substr.size() > str.size())
		return false;
	return std::equal(substr.rbegin(), substr.rend(), str.rbegin(), [](unsigned char a, unsigned char b) {return std::tolower(a) == std::tolower(b); });
}

#endif // !_UTILITY_H_INCLUDED_
