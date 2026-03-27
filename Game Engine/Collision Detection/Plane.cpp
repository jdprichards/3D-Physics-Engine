#include "Plane.h"

Plane::Plane(Entity* entity) : BoundingVolume(entity)
{
	mCollisionType = ECollisionType::Plane;
}

void Plane::UpdateBoundingVolume()
{
	std::vector<Vector3> vertices = mEntity->Template().GetMesh().ScaledVertices(mEntity->Transforms());

	Vector3 p0 = vertices[0];
	Vector3 p1 = vertices[1];
	Vector3 p2 = vertices[2];

	mNormal = Normalise(Cross(p1 - p0, p2 - p0));
	mPlanePoint = p0;
}
