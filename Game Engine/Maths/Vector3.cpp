//=============================================================================
// Vector3 class, encapsulating (x,y, z) coordinate and supporting functions
// supports float (Vector3), double (Vector3d) and int (vector3i)
//=============================================================================


#include "Vector3.h"
#include <cmath>

//=====================
// Member operators
//=====================

// Addition of another vector to this one (e.g. Position + Velocity)
template <typename T> Vector3T<T>& Vector3T<T>::operator+=(const Vector3T<T>& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

// Subtraction of another vector from this one (e.g. Velocity -= Gravity)
template <typename T> Vector3T<T>& Vector3T<T>::operator-=(const Vector3T<T>& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

// Negate this vector (e.g. Velocity = -Velocity)
template <typename T> Vector3T<T> Vector3T<T>::operator-()
{
	//Vector3 vOut = { -x, -y, -z };

	return Vector3T<T>{ -x, -y, -z };
}

// Plus sign in front of vector - called unary positive and usually does nothing. Included for completeness (e.g. velocity = +velocity)
template <typename T> Vector3T<T> Vector3T<T>::operator+()
{
	//Vector3 vOut = { x, y, z };

	return Vector3T<T>{ x, y, z };
}

// Multiply vector by scalar (scales vector) - use of extra template parameter means type of scalar doesn't have to match coordinate type
template <typename T> Vector3T<T>& Vector3T<T>::operator*=(FloatTypeFor<T> s)
{
	x = static_cast<T>(x * s);
	y = static_cast<T>(y * s);
	z = static_cast<T>(z * s);
	return *this;
}

// Divide vector by scalar (scales vector) use of extra template parameter means type of scalar doesn't have to match coordinate type
template <typename T> Vector3T<T>& Vector3T<T>::operator/=(FloatTypeFor<T> s)
{
	x = static_cast<T>(x / s);
	y = static_cast<T>(y / s);
	z = static_cast<T>(z / s);
	return *this;
}

//==========================
// Non-Member Operators
//==========================

// Vector-Vector Addition
template <typename T> Vector3T<T> operator+ (const Vector3T<T>& v, const Vector3T<T>& w)
{
	return { v.x + w.x , v.y + w.y, v.z + w.z };
}

// Vector-Vector Subtraction
template <typename T> Vector3T<T> operator- (const Vector3T<T>& v, const Vector3T<T>& w)
{
	return { v.x - w.x , v.y - w.y, v.z - w.z };
}

// Vector-Scalar Multiplication - use of extra template parameter means type of scalar doesn't have to match coordinate type
template <typename T> Vector3T<T> operator*(const Vector3T<T>& v, FloatTypeFor<T> s)
{
	return { static_cast<T>(v.x * s), static_cast<T>(v.y * s), static_cast<T>(v.z * s) };
}
template <typename T> Vector3T<T> operator*(FloatTypeFor<T> s, const Vector3T<T>& v)
{
	return { static_cast<T>(v.x * s), static_cast<T>(v.y * s), static_cast<T>(v.z * s) };
}

template<typename T>Vector3T<T> operator*(const Vector3T<T>& v1, const Vector3T<T>& v2)
{
	Vector3T<T> outV;
	outV.x = v1.x * v2.x;
	outV.y = v1.y * v2.y;
	outV.z = v1.z * v2.z;
	return outV;
}

// Vector-Scalar Division - use of extra template parameter means type of scalar doesn't have to match coordinate type
template <typename T> Vector3T<T> operator/(const Vector3T<T>& v, FloatTypeFor<T> s)
{
	return { static_cast<T>(v.x / s), static_cast<T>(v.y / s), static_cast<T>(v.z / s) };

}

template <typename T> Vector3T<T> operator/(const Vector3T<T>& v1, const Vector3T<T>& v2)
{
	return { static_cast<T>(v1.x / v2.x), static_cast<T>(v1.y / v2.y), static_cast<T>(v1.z / v2.z) };
}

//==========================
// Other Member Functions
//==========================

// Returns Length of a Vector
template <typename T> FloatTypeFor<T> Vector3T<T>::Length() const
{
	return static_cast<FloatTypeFor<T>>(std::sqrt(x * x + y * y + z * z));
}

template <typename T> FloatTypeFor<T> Vector3T<T>::LengthSq() const
{
	return static_cast<FloatTypeFor<T>>(x * x + y * y + z * z);
}

//==========================
// Non-Member Functions
//==========================

template <typename T> bool operator== (const Vector3T<T>& v1, const Vector3T<T>& v2)
{
	return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z);
}

// Dot Product of Two given Vectors (Order not Importatnt) - non-member version
template <typename T> T Dot(const Vector3T<T>& v1, const Vector3T<T>& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

// Cross Product of Two given Vectors (Order is Important) - Non-Member Version
template <typename T> Vector3T<T> Cross(const Vector3T<T>& v1, const Vector3T<T>& v2)
{
	return { v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x };
}



template <typename T> Vector3T<T> Normalise(const Vector3T<T>& v)
{
	T lengthSq = v.x * v.x + v.y * v.y + v.z * v.z;

	// Can't normalise zero length vector
	if (IsZero(lengthSq))
		return { 0,0,0 };
	else
	{
		T invLength = InvSqrt(lengthSq);
		return { v.x * invLength, v.y * invLength, v.z * invLength };
	}
}

template<typename T> bool IsEqual(const Vector3T<T>& v1, const Vector3T<T>& v2)
{
	return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z);
}

template<typename T> T AngleBetween(const Vector3T<T>& vector1, const Vector3T<T>& vector2)
{
	T dot = Dot(vector1, vector2);
	return std::acos(dot / vector1.Length() * vector2.Length());
}

template<typename T> void SetNearZero(Vector3T<T>& vector)
{
	if (IsZero(vector.x))
		vector.x = 0.0f;
	if (IsZero(vector.y))
		vector.y = 0.0f;
	if (IsZero(vector.z))
		vector.z = 0.0f;
}

//================================================================================================
// Template Instatiation
//================================================================================================
// Instantiate this template calss for specific numeric types. Prevents use of other types and
// allows us to put code in this cpp file (normally template code must be all in header file)

template class Vector3T<float>;  // Vector3 or Vector3f
template class Vector3T<double>; // Vector3d
template class Vector3T<int>;    // Vector3i

// Also need to instantiate all the non-member template functions
template Vector3 operator+(const Vector3& v, const Vector3& w);
template Vector3 operator-(const Vector3& v, const Vector3& w);
template Vector3 operator*(const Vector3& v, float s);
template Vector3 operator*(const Vector3& v1, const Vector3& v2);
template Vector3 operator*(float s, const Vector3& v);
template Vector3 operator/(const Vector3& v, float s);
template Vector3 operator/(const Vector3& v1, const Vector3& v2);
template bool operator==(const Vector3& v1, const Vector3& v2);

template float   Dot(const Vector3& v1, const Vector3& v2);
template Vector3 Cross(const Vector3& v1, const Vector3& v2);
template Vector3 Normalise(const Vector3& v);
template bool IsEqual(const Vector3& v1, const Vector3& v2);
template float AngleBetween(const Vector3& v1, const Vector3& v2);
template void SetNearZero(Vector3& vector);

template Vector3d operator+(const Vector3d& v, const Vector3d& w);
template Vector3d operator-(const Vector3d& v, const Vector3d& w);
template Vector3d operator*(const Vector3d& v, double s);
template Vector3d operator*(const Vector3d& v1, const Vector3d& v2);
template Vector3d operator*(double s, const Vector3d& v);
template Vector3d operator/(const Vector3d& v, double s);
template Vector3d operator/(const Vector3d& v1, const Vector3d& v2);
template bool operator==(const Vector3d& v1, const Vector3d& v2);


template double   Dot(const Vector3d& v1, const Vector3d& v2);
template Vector3d Cross(const Vector3d& v1, const Vector3d& v2);
template Vector3d Normalise(const Vector3d& v);
template bool IsEqual(const Vector3d& v1, const Vector3d& v2);
template double AngleBetween(const Vector3d& v1, const Vector3d& v2);
template void SetNearZero(Vector3d& vector);


template Vector3i operator+(const Vector3i& v, const Vector3i& w);
template Vector3i operator-(const Vector3i& v, const Vector3i& w);
template Vector3i operator*(const Vector3i& v, float s);
template Vector3i operator*(const Vector3i& v1, const Vector3i& v2);
template Vector3i operator*(float s, const Vector3i& v);
template Vector3i operator/(const Vector3i& v, float s);
template Vector3i operator/(const Vector3i& v1, const Vector3i& v2);
template bool operator==(const Vector3i& v1, const Vector3i& v2);

template int   Dot(const Vector3i& v1, const Vector3i& v2);
template Vector3i Cross(const Vector3i& v1, const Vector3i& v2);
template bool IsEqual(const Vector3i& v1, const Vector3i& v2);
template int AngleBetween(const Vector3i& v1, const Vector3i& v2);

// Normalise does not makes sense for Vector3i (integer coordinates) so not included
