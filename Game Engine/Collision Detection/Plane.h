#ifndef _PLANE_H_
#define _PLANE_H_

#include "BoundingVolume.h"

class Plane : public BoundingVolume
{
public:
	Plane(Entity* entity);

private:
	Vector3 mNormal;
	Vector3 mPlanePoint;

public:
	void UpdateBoundingVolume();

	Vector3& Normal() { return mNormal; } // Normal of the plane
	Vector3& PlanePoint() { return mPlanePoint; } // Point on plane
};

#endif // !_PLANE_H_
