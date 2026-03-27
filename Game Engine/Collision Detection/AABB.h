#ifndef _AABB_H_
#define _AABB_H_

#include "BoundingVolume.h";

class AABB : public BoundingVolume
{
public:
	AABB(Entity* entity);

private:
	Vector3 mMinPoint, mMaxPoint;
	Vector3 mMinAABB, mMaxAABB;

public:

	Vector3& MinPoint() { return mMinPoint; }
	Vector3& MaxPoint() { return mMaxPoint; }

	Vector3& MinAABB() { return mMinAABB; }
	Vector3& MaxAABB() { return mMaxAABB; }

	void UpdateBoundingVolume();
};

#endif // !_AABB_H_
