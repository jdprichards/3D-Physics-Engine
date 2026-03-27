#include "Matrix3x3.h"
#include <vector>

// Vector-Matrix Multiplication, transform the Vector by the Matrix
// Use the number functions TransformPoint or TransformVector if you have Vector3 values to be transformed
template<typename T> Vector3T<T> operator*(const Vector3T<T>& v, const Matrix3x3T<T>& m)
{
	Vector3T<T> vOut;
	
	vOut.x = v.x * m.e00 + v.y * m.e10 + v.z * m.e20;
	vOut.y = v.x * m.e01 + v.y * m.e11 + v.z * m.e21;
	vOut.z = v.x * m.e02 + v.y * m.e12 + v.z * m.e22;

	return vOut;
}

template<typename T> Vector3T<T> operator*(const Matrix3x3T<T>& m, const Vector3T<T>& v)
{
	Vector3T<T> vOut;
	vOut.x = m.e00 * v.x + m.e10 * v.y + m.e20 * v.z;
	vOut.y = m.e01 * v.x + m.e11 * v.y + m.e21 * v.z;
	vOut.z = m.e02 * v.x + m.e12 * v.y + m.e22 * v.z;

	return vOut;
}

template<typename T> Matrix3x3T<T> operator* (const Matrix3x3T<T>& m1, const Matrix3x3T<T>& m2)
{
	Matrix3x3T<T> mOut;

	mOut.e00 = m1.e00 * m2.e00 + m1.e01 * m2.e10 + m1.e02 * m2.e20;
	mOut.e01 = m1.e00 * m2.e01 + m1.e01 * m2.e11 + m1.e02 * m2.e21;
	mOut.e02 = m1.e00 * m2.e02 + m1.e01 * m2.e12 + m1.e02 * m2.e22;

	mOut.e10 = m1.e10 * m2.e00 + m1.e11 * m2.e10 + m1.e12 * m2.e20;
	mOut.e11 = m1.e10 * m2.e01 + m1.e11 * m2.e11 + m1.e12 * m2.e21;
	mOut.e12 = m1.e10 * m2.e02 + m1.e11 * m2.e12 + m1.e12 * m2.e22;

	mOut.e20 = m1.e20 * m2.e00 + m1.e21 * m2.e10 + m1.e22 * m2.e20;
	mOut.e21 = m1.e20 * m2.e01 + m1.e21 * m2.e11 + m1.e22 * m2.e21;
	mOut.e22 = m1.e20 * m2.e02 + m1.e21 * m2.e12 + m1.e22 * m2.e22;

	return mOut;
}

template<typename T> Matrix3x3T<T> operator*=(const Matrix3x3T<T>& m1, const Matrix3x3T<T>& m2)
{
	Matrix3x3T<T> mOut;

	mOut.e00 = m1.e00 * m2.e00 + m1.e01 * m2.e10 + m1.e02 * m2.e20;
	mOut.e01 = m1.e00 * m2.e01 + m1.e01 * m2.e11 + m1.e02 * m2.e21;
	mOut.e02 = m1.e00 * m2.e02 + m1.e01 * m2.e12 + m1.e02 * m2.e22;

	mOut.e10 = m1.e10 * m2.e00 + m1.e11 * m2.e10 + m1.e12 * m2.e20;
	mOut.e11 = m1.e10 * m2.e01 + m1.e11 * m2.e11 + m1.e12 * m2.e21;
	mOut.e12 = m1.e10 * m2.e02 + m1.e11 * m2.e12 + m1.e12 * m2.e22;

	mOut.e20 = m1.e20 * m2.e00 + m1.e21 * m2.e10 + m1.e22 * m2.e20;
	mOut.e21 = m1.e20 * m2.e01 + m1.e21 * m2.e11 + m1.e22 * m2.e21;
	mOut.e22 = m1.e20 * m2.e02 + m1.e21 * m2.e12 + m1.e22 * m2.e22;

	return mOut;
}

template<typename T> Vector3T<T> operator*=(const Vector3T<T>& v, const Matrix3x3T<T>& m)
{
	Vector3T<T> vOut;
	vOut.x = m.e00 * v.x + m.e10 * v.y + m.e20 * v.z;
	vOut.y = m.e01 * v.x + m.e11 * v.y + m.e21 * v.z;
	vOut.z = m.e02 * v.x + m.e12 * v.y + m.e22 * v.z;

	return vOut;
}

template<typename T> Matrix3x3T<T> Cross(const Vector3T<T>& v)
{
	Matrix3x3T<T> mOut =
	{
		0.0, -v.z, v.y,
		v.z, 0.0f, -v.x,
		-v.y, v.x, 0.0f
	};

	return mOut;
}

template<typename T> Matrix3x3T<T> MatrixScaling3x3(const Vector3T<T>& s)
{
	return Matrix3x3T<T> 
	{
		s.x, 0, 0,
		0, s.y, 0,
		0, 0, s.z
	};
}

template<typename T> Matrix3x3T<T> MatrixScaling3x3(T s)
{
	return Matrix3x3T<T>
	{
		s, 0, 0,
		0, s, 0,
		0, 0, s
	};
}


template<typename T> Matrix3x3T<T>& Matrix3x3T<T>::Transpose()
{
	std::swap(e01, e10);
	std::swap(e20, e02);
	std::swap(e12, e21);

	return *this;
}

template<typename T> Matrix3x3T<T> InverseAffine(const Matrix3x3T<T>& m)
{
	T det =
		m.e00 * (m.e11 * m.e22 - m.e12 * m.e21) -
		m.e01 * (m.e10 * m.e22 - m.e12 * m.e20) +
		m.e02 * (m.e10 * m.e21 - m.e11 * m.e20);

	T invDet = 1.0f / det;

	Matrix3x3T<T> invMatrix;

	invMatrix.e00 = (m.e11 * m.e22 - m.e12 * m.e21) * invDet;
	invMatrix.e01 = (m.e02 * m.e21 - m.e01 * m.e22) * invDet;
	invMatrix.e02 = (m.e01 * m.e12 - m.e02 * m.e11) * invDet;

	invMatrix.e10 = (m.e12 * m.e20 - m.e10 * m.e22) * invDet;
	invMatrix.e11 = (m.e00 * m.e22 - m.e02 * m.e20) * invDet;
	invMatrix.e12 = (m.e10 * m.e02 - m.e00 * m.e12) * invDet;
	
	invMatrix.e20 = (m.e10 * m.e21 - m.e20 * m.e11) * invDet;
	invMatrix.e21 = (m.e20 * m.e01 - m.e00 * m.e21) * invDet;
	invMatrix.e22 = (m.e00 * m.e11 - m.e10 * m.e01) * invDet;

	return invMatrix;
}

template<typename T> Matrix3x3T<T> Transpose(Matrix3x3T<T>& matrix)
{
	std::swap(matrix.e01, matrix.e10);
	std::swap(matrix.e02, matrix.e20);
	std::swap(matrix.e12, matrix.e21);

	return matrix;
}

template <typename T> const Matrix3x3T<T> Transpose(const Matrix3x3T<T>& matrix)
{
	Matrix3x3T<T> outM;

	outM = matrix;

	std::swap(outM.e01, outM.e10);
	std::swap(outM.e02, outM.e20);
	std::swap(outM.e12, outM.e21);

	return outM;
}

template class Matrix3x3T<float>; // Matrix4x4 or Matrix4x4f
template class Matrix3x3T<double>; // Matrix4x4d

template Vector3 operator* (const Vector3& v, const Matrix3x3& m);
template Vector3 operator* (const Matrix3x3& m, const Vector3& v);
template Matrix3x3 operator* (const Matrix3x3& m1, const Matrix3x3& m2);

template Matrix3x3 operator*= (const Matrix3x3& v, const Matrix3x3& m);
template Vector3 operator*= (const Vector3& v, const Matrix3x3& m );


template Matrix3x3 Cross(const Vector3& v);
template Matrix3x3 MatrixScaling3x3(const Vector3& s);
template Matrix3x3 MatrixScaling3x3(float s);

template Matrix3x3 InverseAffine(const Matrix3x3& m);

template Matrix3x3 Transpose(Matrix3x3& matrix);
template const Matrix3x3 Transpose(const Matrix3x3& matrix);


template Vector3d operator*(const Vector3d& v, const Matrix3x3d& m);
template Vector3d operator*(const Matrix3x3d& m, const Vector3d& v);
template Matrix3x3d operator* (const Matrix3x3d& m1, const Matrix3x3d& m2);

template Matrix3x3d operator*=(const Matrix3x3d& v, const Matrix3x3d& m);
template Vector3d operator*= (const Vector3d& v, const Matrix3x3d& m);


template Matrix3x3d Cross(const Vector3d& v);
template Matrix3x3d MatrixScaling3x3(const Vector3d& s);
template Matrix3x3d MatrixScaling3x3(double s);

template Matrix3x3d InverseAffine(const Matrix3x3d& m);

template Matrix3x3d Transpose(Matrix3x3d& matrix);
template const Matrix3x3d Transpose(const Matrix3x3d& matrix);

