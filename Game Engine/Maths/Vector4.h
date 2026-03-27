//=============================================================================
// Vector4 class, encapsulating (x,y,z,w) coordinates and supporting functions
// Used for pointers and vectors when they are being multiplied by 4x4 matrices
// Supports float (Vector4), double (Vector4d)
//=============================================================================
// Template classes usually put all code in header file. However, here we use explicit template instantiation at the
// end of the .cpp file (see the comment there), which allows us to put the code in the cpp file and just the class
// interface here, just like an ordinary non-template class

#ifndef _VECTOR4_H_DEFINED_
#define _VECTOR4_H_DEFINED_

#include "Vector3.h"

// Template class to support float, double, or int coordinates. Do not use this type, use the simpler types below
template <typename T> class Vector4T;

// Define convenient names for vectors of different types to avoid the messy angle bracket synatx
using Vector4f = Vector4T<float>; // Vector4 with float coords
using Vector4d = Vector4T<double>; // Vector4 with double coords
using Vector4 = Vector4f; // Simple name for Vector4 with float coords
// Not supporting int Vector4

// Full Declaration
template <typename T> class Vector4T
{
// Allow public access for such a simple, well-defined class
public:
	T x;
	T y;
	T z;
	T w;


	//==================
	// Constructors
	//==================
	
	// Default Constructor - Leaves values uninitialised (for performance)
	#pragma warning(suppress: 26495) // Disalbe warning about constructor leaving things uninitialised ("suppress affects next line only)
	Vector4T<T>() {}

	// Construct with 4 values
	Vector4T<T>(const T xIn, const T yIn, const T zIn, const T wIn)
		: x(xIn), y(yIn), z(zIn), w(wIn) {}

	// Construct using a pointer to four values
	explicit Vector4T(const T* elts) // explict means don't allow conversion from pointer to Vector4 without writing constructor name
		// i.e. Vector4 v = somepointer; NOT ALLOWED
		// Vector3 v = Vector4(somepointer); ALLOWED
		// See comment on similar constructor in Matrix4x4 class regarding this kind of constructor
		: x(elts[0]), y(elts[1]), z(elts[2]), w(elts[3]) {}

	// Cast to Vector3 - allows use of Vector3 methods on the x,y,z members only (e.g. Dot Product)
	operator Vector3T<T>()
	{
		return Vector3T<T>(&x);
	}
};

// NO other content for Vector4. Although the usual addition, subtraction, Dot, Cross, Length etc. functions can
// be supporte, the 4th value we usually have (1 for pointer or 0 for vectors) makes those functions perform
// non-intuitvely. If you want those features cast to a Vector3 - an appropriate cast operator is provided


#endif // !_VECTOR4_H_DEFINED_
