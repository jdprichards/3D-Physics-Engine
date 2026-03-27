//===============================
// Maths Convenience Functions
//===============================


#include "MathHelpers.h"

//================
// Float work
//================

// Test if a float value is approximately equal to 0
// Espilon value is the range around zero that is considered equal to zero
const float EPSILON32 = 0.5e-6f;
template<> bool IsZero<float>(const float x)
{
	return std::abs(x) < EPSILON32;
}

// Test is double value is approximately 0
// Epsilon value is the range around zero that is considered equal to zero
const double EPSILON64 = 0.5e-15;
template<> bool IsZero<double>(const double x)
{
	return std::abs(x) < EPSILON64;
}

//=======================
// Random Numbers
//=======================

// Return a random interger from a to b (inclusive)
// Can only return up to RAND_MAX different values, spread everly across the given range
// RAND_MAX is defined in stdlib.h and is compiler specific (32767 in vs)
template<> int Random<int>(const int a, const int b)
{
	// Could just use a + rand() % (b-a), but using a more complex form to allow the range
	// to exceed RAND_MAX and still return values spread across the range
	
	int t = (b - a + 1) * rand();
	return t == 0 ? a : a + (t - 1) / RAND_MAX;
}


// Return random float from a to b (inclusive)
// Can on;y reutrn up to RAND_MAX different values, spread evenly across the given range
// RAND_MAX is defined in stdlib.h and is compiler-specific
template<> float Random<float>(const float a, const float b)
{
	return a + (b - a) * (static_cast<float>(rand()) / RAND_MAX);
}

// Return random double from a to b (inclusive)
// Can only return up to RAND_MAX different values, spread evenly accross the given range
// RAND_MAX is definied in stdlib.h and is compiler specific
template<> double Random<double>(const double a, const double b)
{
	return a + (b - a) * (static_cast<double>(rand() / RAND_MAX));
}