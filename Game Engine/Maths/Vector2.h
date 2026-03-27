//=============================================================================
// Vector2 class, encapsulating (x,y) coordinate and supporting functions
// supports float (Vector2), double (Vector2d) and int (vector2i)
//=============================================================================
// Template classes usual put all code in header file. However, here we use explicit template instantiation at the
// end of the .cpp file, allows us to put the code in the cpp file and just the class
// interface here, kust like ordinary non template class

#ifndef _VECTOR2_H_DEFINED_
#define _VECTOR2_H_DEFINED_

#include "MathHelpers.h"

// Template class to support float, double or int coordinates. Do not use this typename, use the simpler types below
template <typename T> class Vector2T;

// Define convenient names for vectors of different types to avoid messy angle brackets
using Vector2i = Vector2T<int>; // 2D vector with int coordinates
using Vector2f = Vector2T<float>; // 2D vector with float coordinates
using Vector2d = Vector2T<double>; // 2D vector with double coordinates
using Vector2 = Vector2f; // Single name for 2D vector with float coordinates

// Full decleration
template<typename T> class Vector2T
{
	// Allow public access for which simple, well defined class
public:
	// Vector Compoenets
	T x;
	T y;

	//==================
	// Constructors
	//==================

	// Default constructor
	#pragma warning(supress: 26495) // disable warning about constructor leaving thing uninitalised
	Vector2T() {}

	// Construct with 2 values
	Vector2T(const T xIn, const T yIn) : x(xIn), y(yIn) {}

	// Construct with pointer to 2 values
	explicit Vector2T(const T* elts) // explicit doesn't allow conversion from pointer to Vector2 without writing the constructor name
		// i.e. Vector2 v = somepointer; // Not allowed
		// Vector2 v = Vector2(somepointer) // OK, explicittly asked for constructor
		// See comment earlier on similar constructor in Matrix4x4 class regarding this kind of constructor
		: x(elts[0]), y(elts[1]) {};

	//=====================
	// Member operators
	//=====================

	// Addition of another vector to this one, (e.g. Position += Velocity)
	Vector2T& operator+=(const Vector2T& v);

	// Subtraction of anohter vector from this one, (e.g. Velocity -= Gravity)
	Vector2T& operator-=(const Vector2T& v);

	// Negate this vector (e.g Velocity = -Velocity)
	Vector2T& operator-();

	// Plus sign infront of vector - called unary positive and usually does nothing. Included for completeness (e.g. Velocity = +Velocity)
	Vector2T& operator+();

	// Mutiply vectors by sclar (scales vector)
	// Integer vectors can be multiplied by a float but the resulting vector will be rounded to integers
	Vector2T& operator*= (FloatTypeFor<T> s);

	// Divide vector by scaler (scales vectors)
	// Integer vector can be divided by a float but the resulting vector will be rounded to integers
	Vector2T& operator/= (FloatTypeFor<T> s);

	//==========================
	// Other Member Functions
	//==========================

	// Returns the length of the vector (return value always a floating point, when with an integer vector)
	FloatTypeFor<T> Length() const;


};

//==========================
// Non-Member Operators
//==========================

// Vector - Vector addition
template<typename T> Vector2T<T> operator+ (const Vector2T<T>& v, const Vector2T<T>& w);

// Vecotr - Vector subtraction
template <typename T> Vector2T<T> operator- (const Vector2T<T>& v, const Vector2T<T>& w);

// Vector - Scalar Multiplication and Division
template <typename T> Vector2T<T> operator* (const Vector2T<T>& v, FloatTypeFor<T> s);
template <typename T> Vector2T<T> operator* (FloatTypeFor<T> s, const Vector2T<T>& v);
template <typename T> Vector2T<T> operator/ (const Vector2T<T>& v, FloatTypeFor<T> s);

//==========================
// Non-Member Functions
//==========================

// Distance between two Vector2 points
template<typename T> FloatTypeFor<T> Distance(const Vector2T<T>& v1, const Vector2T<T>& v2)
{
	return (v2 - v1).Length();
}

// Dot proudct of two given vectors (order not important)
template <typename T> T Dot(const Vector2T<T>& v1, const Vector2T<T>& v2);

// Return unit length vector in the same direction as given one (not supported for int coordinates Vector2i)
template <typename T> Vector2T<T> Normalise(const Vector2T<T>& v);


#endif // !_VECTOR2_H_DEFINED_
