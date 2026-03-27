#ifndef _Bounding_Sphere_H_
#define _Bounding_Sphere_H_

#include "BoundingVolume.h"

class BoundingSphere : public BoundingVolume
{
public:
	BoundingSphere(Entity* entity);

private:
	float mRadius;

public:
	float& Radius() { return mRadius; }
	void UpdateBoundingVolume();

};

#endif _Bounding_Sphere_H_