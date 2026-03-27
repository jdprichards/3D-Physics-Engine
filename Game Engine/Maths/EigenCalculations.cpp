#include "EigenCalculations.h"

template<typename T> Matrix3x3T<T> CalculateCovarienceMatrix(std::vector<Vector3T<T>> vertices, Vector3T<T> center)
{
	Matrix3x3T<T> covarienceMatrix =
	{
		0, 0, 0,
		0, 0, 0,
		0, 0, 0
	};

	///std::vector<Vector3> vertices = entity->Template().GetMesh().ScaledVertices(entity->Transforms());

	float numVertices = vertices.size();
	float invNum = 1 / numVertices;

	Vector3T<T> distribution = { 0,0,0 };

	for (auto& vertex : vertices)
	{
		covarienceMatrix.e00 += invNum * Square(vertex.x - center.x);
		covarienceMatrix.e01 += invNum * (vertex.x - center.x) * (vertex.y - center.y);
		covarienceMatrix.e02 += invNum * (vertex.x - center.x) * (vertex.z - center.z);

		covarienceMatrix.e10 += invNum * (vertex.y - center.y) * (vertex.x - center.x);
		covarienceMatrix.e11 += invNum * Square(vertex.y - center.y);
		covarienceMatrix.e12 += invNum * (vertex.y - center.y) * (vertex.z - center.z);

		covarienceMatrix.e20 += invNum * (vertex.z - center.z) * (vertex.x - center.x);
		covarienceMatrix.e21 += invNum * (vertex.z - center.z) * (vertex.y - center.y);
		covarienceMatrix.e22 += invNum * Square(vertex.z - center.z);
	}


	// Checks to see if any elements are near zero if so sets them to zero
	covarienceMatrix.e00 = IsZero(covarienceMatrix.e00) ? 0 : covarienceMatrix.e00;
	covarienceMatrix.e01 = IsZero(covarienceMatrix.e01) ? 0 : covarienceMatrix.e01;
	covarienceMatrix.e02 = IsZero(covarienceMatrix.e02) ? 0 : covarienceMatrix.e02;

	covarienceMatrix.e10 = IsZero(covarienceMatrix.e10) ? 0 : covarienceMatrix.e10;
	covarienceMatrix.e11 = IsZero(covarienceMatrix.e11) ? 0 : covarienceMatrix.e11;
	covarienceMatrix.e12 = IsZero(covarienceMatrix.e12) ? 0 : covarienceMatrix.e12;

	covarienceMatrix.e20 = IsZero(covarienceMatrix.e20) ? 0 : covarienceMatrix.e20;
	covarienceMatrix.e21 = IsZero(covarienceMatrix.e21) ? 0 : covarienceMatrix.e21;
	covarienceMatrix.e22 = IsZero(covarienceMatrix.e22) ? 0 : covarienceMatrix.e22;


	return covarienceMatrix;
}

// A Matrix factorization technique to Decompose a Matrix (Q), into a Product of an Orthoginal Matrix (O)
// and a Upper Triangle matrix (R).
// Where:
// - Transpose(Q)*Q is an Identity Matrix
// - R_(i,j) = 0 (For i > j): R_(row, column)
template<typename T> void QR_Decomposition(const Matrix3x3T<T>& M, Matrix3x3T<T>& Q, Matrix3x3T<T>& R)
{
	unsigned int n = 3;

	std::vector<Vector3T<T>> x;
	std::vector<Vector3T<T>> v;

	Matrix3x3T<T> transM = Transpose(M);
	for (unsigned int i = 0; i < n; i++)
	{
		x.push_back(transM.Row(i));
		v.push_back({ 0, 0, 0 });
	}

	for (unsigned int i = 0; i < n; i++)
	{
		v[i] += x[i];
		for (unsigned int j = 0; j < i; j++)
		{
			if (IsZero(v[j].Length()))
			{
				v[i] += {0, 0, 0};
				continue;
			}

			v[j] -= (Dot(x[i], v[j]) / v[j].Length()) * v[j];
		}
	}

	for (unsigned int i = 0; i < n; i++)
	{
		if (IsZero(v[i].Length()))
		{
			Transpose(Q).Row(i) = Vector3T<T>({ 0,0,0 });
			continue;
		}
		Transpose(Q);

		Q.Row(i) = v[i] / v[i].Length();
	}

	R = Transpose(Q) * M;
	R.e10 = 0;
	R.e21 = 0;
	R.e20 = 0;

	printf("TEST");
}

template<typename T> std::pair<Vector3T<T>, Matrix3x3T<T>> EignenDecoposition(Matrix3x3T<T> M)
{
	Matrix3x3T<T> Q =
	{
		0, 0, 0,
		0, 0, 0,
		0, 0, 0
	};

	Matrix3x3T<T> R =
	{
		0, 0, 0,
		0, 0, 0,
		0, 0, 0
	};

	Matrix3x3T<T> Q_k = Matrix3x3T<T>(
		{
			1.0, 1.0, 1.0,
			1.0, 1.0, 1.0,
			1.0, 1.0, 1.0
		});

	QR_Decomposition(M, Q, R);
	M = R * Q;
	Q_k *= Q;

	Vector3T<T> eigenVector = { M.Row(0).x, M.Row(1).y, M.Row(2).z };

	return { eigenVector, Q_k };
}

template<typename T> bool FindSide(Vector3T<T> a, Vector3T<T> b, Vector3T<T> point)
{
	//Vector3T<T> AB = b - a;
	//Vector3T<T> AP = point - a;
	//Vector3T<T> right = Cross(AB, BP);

	//return Dot(right, Inverse(AB));

	return false;
}

template<typename T> T LineDistance(Vector3T<T> a, Vector3T<T> b, Vector3T<T> point)
{
	Vector3T<T> plane = b - a;

	T distance = (Dot(plane, point) / plane.Length());

	return distance;
}

// To be implemented Later
template<typename T> void FindHull(std::vector<Vector3T<T>> pointCloud, Vector3T<T> lineStart, Vector3T<T> lineEnd, std::vector<Vector3T<T>>& hull)
{
	if (pointCloud.empty())
		return;

	T furthestDistance = -INFINITY;
	Vector3T<T> furthest = { 0,0,0 };

	// Finding Point with max distance
	for (auto& point : pointCloud)
	{
		if (LineDistance(lineStart, lineEnd, point) > furthestDistance)
			furthest = point;
	}

	std::vector<Vector3T<T>> tempHull;
	for (auto& point : hull)
	{
		tempHull.push_back(point);

		if (point == lineStart)
		{
			tempHull.push_back(furthest);
		}
	}

	std::vector<Vector3T<T>> rightSideCloud;
	std::vector<Vector3T<T>> leftSideCloud;

	FindHull(rightSideCloud, lineStart, furthest, hull);
	FindHull(leftSideCloud, furthest, lineEnd, hull);

	hull = tempHull;
}

template<typename T> void QuickHull(std::vector<Vector3T<T>> pointCloud, std::vector<Vector3T<T>>& hull)
{
	int ind = -1;
	int max_dist = 0;

	Vector3T<T> lowestX = { INFINITY, INFINITY, INFINITY };
	Vector3T<T> highestX = { -INFINITY, -INFINITY, -INFINITY };

	for (auto& point : pointCloud)
	{
		if (point.x > highestX.x)
			highestX = point;
		
		if (point.x < lowestX.x)
			lowestX = point;
	}

	hull.push_back(lowestX);
	hull.push_back(highestX);

	Vector3T<T> line = highestX - lowestX;

	FindHull(pointCloud, lowestX, highestX, hull);

}

template Matrix3x3 CalculateCovarienceMatrix(std::vector<Vector3> vertices, Vector3 center);
template void QR_Decomposition(const Matrix3x3& M, Matrix3x3& Q, Matrix3x3& R);
template std::pair<Vector3, Matrix3x3> EignenDecoposition(Matrix3x3 M);


template Matrix3x3d CalculateCovarienceMatrix(std::vector<Vector3d> vertices, Vector3d center);
template void QR_Decomposition(const Matrix3x3d& M, Matrix3x3d& Q, Matrix3x3d& R);
template std::pair<Vector3d, Matrix3x3d> EignenDecoposition(Matrix3x3d M);

template bool FindSide(Vector3 a, Vector3 b, Vector3 point);
template float LineDistance(Vector3 a, Vector3 b, Vector3 point);
template void FindHull(std::vector<Vector3> pointCloud, Vector3 lineStart, Vector3 lineEnd, int side, std::vector<Vector3>& hull);
template void QuickHull(std::vector<Vector3> pointCloud, std::vector<Vector3>& hull);

template bool FindSide(Vector3d a, Vector3d b, Vector3d point);
template double LineDistance(Vector3d a, Vector3d b, Vector3d point);
template void FindHull(std::vector<Vector3d> pointCloud, Vector3d lineStart, Vector3d lineEnd, int side, std::vector<Vector3d>& hull);
template void QuickHull(std::vector<Vector3d> pointCloud, std::vector<Vector3d>& hull);