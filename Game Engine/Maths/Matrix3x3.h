#ifndef _MATRIX3X3_H_DEFINED_
#define _MATRIX3X3_H_DEFINED_

#include "Vector3.h"

// Template class to support float or double values
template <typename T> class Matrix3x3T;

// Define comvenient names for matrices of different types to avoid angular brackets in main code
using Matrix3x3f = Matrix3x3T<float>;
using Matrix3x3d = Matrix3x3T<double>;
using Matrix3x3 = Matrix3x3f;

template <typename T> class Matrix3x3T
{
public:
	// Matrix elements
	T e00, e01, e02;
	T e10, e11, e12;
	T e20, e21, e22;

	//==============
	// Contructors
	//==============

	// Default constuctor - leaves values uninitialised
	#pragma warning(suppress : 26495)
	Matrix3x3T<T>() {}

	// Construct with 9 values
	Matrix3x3T<T>
		(
			T v00, T v01, T v02,
			T v10, T v11, T v12,
			T v20, T v21, T v22
		) :
		e00(v00), e01(v01), e02(v02),
		e10(v10), e11(v11), e12(v12),
		e20(v20), e21(v21), e22(v22) {}

	// Constructor using pointer to 9 values
	explicit Matrix3x3T<T>(T* elts)
	{
		*this = *reinterpret_cast<Matrix3x3T<T>*>(elts);
	}

	//=============
	// Data access
	//=============

	// Direct access to values in a row (range 0 -> 2)
	Vector3T<T>& Row(int row)
	{
		return *reinterpret_cast<Vector3T<T>*>(&e00 + row * 3);
	}

	// Direct access to values in a row (range 0 -> 2)
	const Vector3T<T>& Row(int row) const
	{
		return *reinterpret_cast<const Vector3T<T>*>(&e00 + row * 3);
	}


	// Direct access to X axis of the matrix 
	Vector3T<T>& XAxis() { return Row(0); }

	// Direct access to Y axis of the matrix 
	Vector3T<T>& YAxis() { return Row(1); }

	// Direct access to Z axis of the matrix 
	Vector3T<T>& ZAxis() { return Row(2); }

	// Direct access to X axis of the matrix - const reference
	const Vector3T<T>& XAxis() const{ return Row(0); }

	// Direct access to Y axis of the matrix - const reference
	const Vector3T<T>& YAxis() const { return Row(1); }

	// Direct access to Z axis of the matrix - const reference
	const Vector3T<T>& ZAxis() const { return Row(2); }

	// Other functions
	// Transpose the matrix (rows become columns), returns matrix so result can be used in further operations, e.g.
	// Matrix4x4 m3 = Transpose(m1) * m2;
	// Different APIs use different conventions for rows and columns so this operation is sometimes needed
	// It is also used in some mathematical operations
	Matrix3x3T<T>& Transpose();

public:
	//===================
	// Static Members
	//===================

	// Identity matrix, use this like:
	// Matrix3x3 newMatrix = Matrix3x3::Identity;
	static const Matrix3x3T<T> Identity;
};

// Initialise static identity matrix
template <typename T> const Matrix3x3T<T> Matrix3x3T<T>::Identity = {
	1, 0, 0,
	0, 1, 0,
	0, 0, 1
};

template<typename T> Vector3T<T> operator*(const Vector3T<T>& v, const Matrix3x3T<T>& m);
template<typename T> Vector3T<T> operator*(const Matrix3x3T<T>& m, const Vector3T<T>& v);
template<typename T> Matrix3x3T<T> operator* (const Matrix3x3T<T>& m1, const Matrix3x3T<T>& m2);

template<typename T> Vector3T<T> operator*=(const Vector3T<T>& v, const Matrix3x3T<T>& m);
template<typename T> Matrix3x3T<T> operator*=(const Matrix3x3T<T>& m1, const Matrix3x3T<T>& m2);


template<typename T> Matrix3x3T<T> Cross(const Vector3T<T>& v);
template<typename T> Matrix3x3T<T> MatrixScaling3x3(const Vector3T<T>& s);
template<typename T> Matrix3x3T<T> MatrixScaling3x3(const T s);


// Return the inverse of given matrix assuming that it is an affine matrix
// Most commonly used to get a view matrix from the camera's positioning matrix
template<typename T> Matrix3x3T<T> InverseAffine(const Matrix3x3T<T>& m);

// Transpose the matrix (rows become columns)
// Different APIs use different conventions for rows and columns so this operation is sometimes needed
// It is also used in some mathematical operations
template<typename T> Matrix3x3T<T> Transpose(Matrix3x3T<T>& matrix);
template<typename T> const Matrix3x3T<T> Transpose(const Matrix3x3T<T>& matrix);

#endif // !_MATRIX3X3_H_DEFINED_
