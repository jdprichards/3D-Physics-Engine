#include "OBB.h"

OBB::OBB(Entity* entity) : BoundingVolume(entity)
{
	mCollisionType = ECollisionType::OBB;
}

void OBB::UpdateBoundingVolume()
{
	std::vector<Vector3> vertices = mEntity->Template().GetMesh().ScaledVertices(mEntity->Transforms());
	//vertices = mEntity->Template().GetMesh().Vertices();

	// Gets rid of repeated vertices
	std::vector<Vector3> tempVertices;
	for (auto& vertice : vertices)
	{
		bool contains = false;
		if (tempVertices.size() > 0)
		{
			for (auto& tempVertice : tempVertices)
			{
				if (vertice == tempVertice)
				{
					contains = true;
					break;
				}
			}

			if (!contains)
				tempVertices.push_back(vertice);
		}
		else
		{
			tempVertices.push_back(vertice);
		}
	}

	std::vector<Vector3> noReplicatedVertices = tempVertices;
	// Implement Later ( calculates just the points on the mesh that are at the edge ) 
	//std::vector<Vector3> hull;
	//QuickHull(mEntity->Template().GetMesh().Vertices(), hull);

	Matrix3x3 covarianceMatrix = CalculateCovarienceMatrix(noReplicatedVertices, mEntity->CenterOfMass());

	auto [eigenValues, eigenVectors] = EignenDecoposition(covarianceMatrix);

	std::vector<Vector3> transformedVertices;

	for (auto vertice : vertices)
	{
		transformedVertices.push_back(vertice * eigenVectors);
	}

	Vector3 minExtent = { INFINITY, INFINITY, INFINITY };
	Vector3 maxExtent = { -INFINITY, -INFINITY, -INFINITY };

	for (auto& vertice : transformedVertices)
	{
		if (vertice.x < minExtent.x)
			minExtent.x = vertice.x;
		if (vertice.y < minExtent.y)
			minExtent.y = vertice.y;
		if (vertice.z < minExtent.z)
			minExtent.z = vertice.z;

		if (vertice.x > maxExtent.x)
			maxExtent.x = vertice.x;
		if (vertice.y > maxExtent.y)
			maxExtent.y = vertice.y;
		if (vertice.z > maxExtent.z)
			maxExtent.z = vertice.z;
	}

	std::vector<Vector3> corners =
	{
		{minExtent.x, minExtent.y, minExtent.z},
		{minExtent.x, minExtent.y, maxExtent.z},
		{minExtent.x, maxExtent.y, minExtent.z},
		{minExtent.x, maxExtent.y, maxExtent.z},
		{maxExtent.x, minExtent.y, minExtent.z},
		{maxExtent.x, minExtent.y, maxExtent.z},
		{maxExtent.x, maxExtent.y, minExtent.z},
		{maxExtent.x, maxExtent.y, maxExtent.z},
	};

	std::vector<Vector3> transformedCorners;
	for (const auto& corner : corners)
	{
		transformedCorners.push_back(Vector3(corner * eigenVectors));
	}

	for (auto& corner : transformedCorners)
		corner += mEntity->Position();

	mCorners = transformedCorners;

	axes.clear();
	axes.push_back(Vector3(eigenVectors.e00, eigenVectors.e01, eigenVectors.e02));
	axes.push_back(Vector3(eigenVectors.e10, eigenVectors.e11, eigenVectors.e12));
	axes.push_back(Vector3(eigenVectors.e20, eigenVectors.e21, eigenVectors.e22));

	printf("Test");
}

