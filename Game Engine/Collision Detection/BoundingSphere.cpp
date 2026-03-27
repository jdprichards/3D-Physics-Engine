#include "BoundingSphere.h"

BoundingSphere::BoundingSphere(Entity* entity) : BoundingVolume(entity)
{
	mCollisionType = ECollisionType::BoundingSphere;

	UpdateBoundingVolume();
}

void BoundingSphere::UpdateBoundingVolume()
{
	unsigned int polygonCount = mEntity->Template().GetMesh().PolygonCount();

	std::vector<Vector3> polygonVectices;
	std::vector<Vector3> vertices = mEntity->Template().GetMesh().ScaledVertices(mEntity->Transforms());

	float maxX = -INFINITY;
	float minX = INFINITY;
	float maxY = -INFINITY;
	float minY = INFINITY;
	float maxZ = -INFINITY;
	float minZ = INFINITY;

	for (auto vertex : vertices)
	{
		if (vertex.x > maxX)
			maxX = vertex.x;
		if (vertex.x < minX)
			minX = vertex.x;
		if (vertex.y > maxY)
			maxY = vertex.y;
		if (vertex.y < minY)
			minY = vertex.y;

		if (vertex.z > maxZ)
			maxZ = vertex.z;
		if (vertex.z < minZ)
			minZ = vertex.z;
	}

	mCenter = Vector3({ minX + (maxX - minX) / 2, minY + (maxY - minY) / 2, minZ + (maxZ - minZ) / 2 });
	mRadius = (std::max({ maxX - minX, maxY - minY, maxZ - minZ }) / 2);
}
