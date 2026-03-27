#ifndef _EIGEN_CALCULATIONS_H_
#define _EIGEN_CALCULATIONS_H_

#include <Vector3.h>
#include <Matrix3x3.h>

#include <vector>

class EigenCalculations
{
};

// A technique to calculate the covariance (distribution) of a std::vector<Vector3> of points
// used to calculate the oriented bounding box (OBB) of a entity from its meshs vertices 
template<typename T> Matrix3x3T<T> CalculateCovarienceMatrix(std::vector<Vector3T<T>> vertices, Vector3T<T> center);

// A Matrix factorization technique to Decompose a Matrix (Q), into a Product of an Orthoginal Matrix (O)
// and a Upper Triangle matrix (R). Using the Gram-Schmit Process (https://en.wikipedia.org/wiki/Gram%E2%80%93Schmidt_process)
// Where:
// - Transpose(Q)*Q is an Identity Matrix
// - R_(i,j) = 0 (For i > j): R_(row, column)
template<typename T> void QR_Decomposition(const Matrix3x3T<T>& M, Matrix3x3T<T>& Q, Matrix3x3T<T>& R);
template<typename T> std::pair<Vector3T<T>, Matrix3x3T<T>> EignenDecoposition(Matrix3x3T<T> M);

template<typename T> bool FindSide(Vector3T<T> lineStart, Vector3T<T> lineEnd, Vector3T<T> point);
template<typename T> T LineDistance(Vector3T<T> lineStart, Vector3T<T> lineEnd, Vector3T<T> point);
template<typename T> void QuickHull(std::vector<Vector3T<T>> pointCloud, std::vector<Vector3T<T>>& hull);
template<typename T> void FindHull(std::vector<Vector3T<T>> pointCloud, Vector3T<T> lineStart, Vector3T<T>  lineEnd, int side, std::vector<Vector3T<T>>& hull);

#endif // !_EIGEN_CALCULATIONS_H_
