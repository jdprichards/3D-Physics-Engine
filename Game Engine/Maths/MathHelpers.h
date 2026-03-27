//===============================
// Maths Convenience Functions
//===============================

#ifndef _MATHS_HELPERS_H_DEFINED_
#define _MATHS_HELPERS_H_DEFINED_


#include <cmath>
#include <numbers>
#include <type_traits>

// Special type to handle situations where floating point numbers are exceptionally requiredin integer-based template classes
// For example a Vector2i (2D vector with int coords) needs a length function that returns a float not a int, so this type is used
// Specifically:
//	FloatTypeFor<int> is float
//	FloatTypeFor<float> is float
//	FloatTypeFor<double> is double
template<typename T> using FloatTypeFor = std::conditional_t<std::is_floating_point_v<T>, T, float>;

// Test if a floating point value is approximately 0
template <typename T> bool IsZero(const T x);

// 1 / Sqrt. Used orften (e.g. normalising) can be optimised, so it gets it's own function
// Supports int, float and double. Use of conditional_t ensures int version returns a float result
template<typename T, typename U = std::conditional_t<std::is_integral_v<T>, float, T>> constexpr U InvSqrt(const T x)
{
	return 1 / std::sqrt(x);
}

template<typename T, typename U = std::conditional_t<std::is_integral_v<T>, float, T>> constexpr U Square(const T x)
{
	return x * x;
}

// Pass an angle in degrees, returns the angle in radians
// Support int float and double. Use of conditional_t ensure int versions returns a float result
template <typename T, typename U = std::conditional_t < std::is_integral_v<T>, float, T>> constexpr U ToRadians(T d)
{
	return static_cast<U>(d) * std::numbers::pi_v<U> / 180;
}

// Pass an angle in radians, returns an angle in degrees
// Supports int, float and double. Use of conditional_t ensures int version returns a float result 
template <typename T, typename U = std::conditional_t<std::is_integral_v<T>, float, T>> constexpr U ToDegrees(T r)
{
	return static_cast<U>(r) * 180 / std::numbers::pi_v<U>;
}

// Return random number from a to b (inclusive) - will return int, float or number random number matching type of parameters a & b
template <typename T> T Random(const T a, const T b);

#endif // !_MATHS_HELPERS_H_DEFINED_