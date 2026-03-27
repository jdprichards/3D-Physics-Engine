#include "AABB.h"

AABB::AABB(Entity* entity) : BoundingVolume(entity)
{
	mCollisionType = ECollisionType::AABB;

	UpdateBoundingVolume();
}

void AABB::UpdateBoundingVolume()
{
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

	mMinPoint = { minX, minY, minZ };
	mMaxPoint = { maxX, maxY, maxZ };

	// AABB in object space
	mMinAABB = mMinPoint;
	mMaxAABB = mMaxPoint;

	// AABB in world-space
	mMinPoint += mEntity->Position();
	mMaxPoint += mEntity->Position();

	mCenter = (mMaxPoint + mMinPoint) / 2;
}
