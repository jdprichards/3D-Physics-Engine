//=============================================================================
// Vector3 class, encapsulating (x,y, z) coordinate and supporting functions
// supports float (Vector3), double (Vector3d) and int (vector3i)
//=============================================================================
// Template classes usual put all code in header file. However, here we use explicit template instantiation at the
// end of the .cpp file, allows us to put the code in the cpp file and just the class
// interface here, kust like ordinary non template class

#ifndef _VECTOR3_H_DEFINED_
#define _VECTOR3_H_DEFINED_

#include "MathHelpers.h"

// Template class to support floa , double or int coordinates. Do not use this typename, use the simpler types below
template <typename T> class Vector3T;

// Define convenient names for vectors of different types to avoid to avoid the messy angle bracket syntax
using Vector3i = Vector3T<int>; // 3D vector with int coordinates
using Vector3f = Vector3T<float>; // 3D vector with float coords
using Vector3d = Vector3T<double>; // 3D vector with double coords
using Vector3 = Vector3f; // Simple name for 3D vector with float coords


// Full declaration
template <typename T> class Vector3T
{
// Allow public access for a simple, well defined class

public:
	T x;
	T y;
	T z;

	//==================
	// Constructors
	//==================

	// Default constructor - leaves values uninitialsed
	#pragma warning(supress: 26495) // disable warning about constructors leaving things uninitialised (suppress affects next line only)
	Vector3T() {}

	// Contruct with 3 values
	Vector3T(const T xIn, const T yIn, const T zIn) : x(xIn), y(yIn), z(zIn) {};

	// Construct using a pointer to three values
	explicit Vector3T(const T* elts) // explict means don't allow conversion from pointer to Vector3 without writing constructor name
		// i.e. Vector3 v = somepointer; NOT ALLOWED
		// Vector3 v = Vector3(somepointer); ALLOWED
		// See comment on similar constructor in Matrix4x4 class regarding this kind of constructor
	: x(elts[0]), y(elts[1]), z(elts[2]) {}


	//=====================
	// Member operators
	//=====================

	// Addition of another vector to this one (e.g. Position += Velocity)
	Vector3T& operator+= (const Vector3T& v);

	// Subtration of another vector from this one (e.g. Velocity -= Gravity)
	Vector3T& operator-= (const Vector3T& v);

	// Negate this vector (e.g. Velocity = -Velocity)
	Vector3T operator- ();

	// Plus sign in front of the vector - call unrary postive sand usually does nothing. Included for completeness
	Vector3T operator+ ();

	// Multiply this vector by a scalar (scales vector)
	// Integer can be multiplied by a float but the resulting vector will be rounded to integers
	Vector3T& operator*= (FloatTypeFor<T> s);

	// Divide this vector by a scalar (scales vector)
	// Integer can be divided by a float but the resulting vector will be rounded to integers
	Vector3T& operator/= (FloatTypeFor<T> s);

	//==========================
	// Other Member Functions
	//==========================

	// Return length of the vector (return value always floating point, even with an integer vector)
	FloatTypeFor<T> Length() const;
	FloatTypeFor<T> LengthSq() const;

};

//==========================
// Non-Member Operators
//==========================

// Equal
template <typename T> bool operator== (const Vector3T<T>& v1, const Vector3T<T>& v2);

// Vector-Vector addition
template <typename T> Vector3T<T> operator+ (const Vector3T<T>& v, const Vector3T<T>& w);

// Vector-Vector subtraction
template <typename T> Vector3T<T> operator- (const Vector3T<T>& v, const Vector3T<T>& w);

// Vector-Scalar multiplication & division
// Integer vector can be mutiplied / divided by a float but the resulting vector will rounded to integers
template <typename T> Vector3T<T> operator* (const Vector3T<T>& v, FloatTypeFor<T> s);
template <typename T> Vector3T<T> operator* (FloatTypeFor<T> s, const Vector3T<T>& v);
template <typename T> Vector3T<T> operator* (const Vector3T<T>& v1, const Vector3T<T>& v2);
template <typename T> Vector3T<T> operator/ (const Vector3T<T>& v, FloatTypeFor<T> s);
template <typename T> Vector3T<T> operator/ (const Vector3T<T>& v1, const Vector3T<T>& v2);

//==========================
// Non-Member Functions
//==========================

// Distance between two Vector3 points
template <typename T> FloatTypeFor<T> Distance(const Vector3T<T>& v1, const Vector3T<T>& v2)
{ 
	return (v2 - v1).Length(); 
}

// Dot product of two given vectors (order not important) - non-member function
template <typename T> T Dot(const Vector3T<T>& v1, const Vector3T<T>& v2);

// Cross product of two given vectors (order is important) - non-member function
template <typename T> Vector3T<T> Cross(const Vector3T<T>& v1, const Vector3T<T>& v2);

// Return unit length vector in same diretion as give one
template <typename T> Vector3T<T> Normalise(const Vector3T<T>& v);

// Check to see if two Vector3s are equal
template <typename T> bool IsEqual(const Vector3T<T>& v1, const Vector3T<T>& v2);

template <typename T> T AngleBetween(const Vector3T<T>& vector1, const Vector3T<T>& vector2);

template <typename T> void SetNearZero(Vector3T<T>& vector);

#endif // !_VECTOR3_H_DEFINED_