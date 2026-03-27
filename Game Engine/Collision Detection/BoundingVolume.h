#ifndef _BOUNDING_VOLUME_H_
#define _BOUNDING_VOLUME_H_

#include "Vector3.h"
#include "Entity.h"
#include "CollisionTypes.h"
#include "EigenCalculations.h"

class Entity;
enum class ECollisionType;

class BoundingVolume
{
public:
	BoundingVolume(Entity* entity);

protected:
	Entity* mEntity;

	Vector3  mCenter;
	ECollisionType mCollisionType;

public:
	virtual void UpdateBoundingVolume() {};
	ECollisionType& GetCollisionType() { return mCollisionType; };

	Vector3& Center() { return mCenter; }
	Vector3 RelativeCenter();

};

#endif // !_BOUNDING_VOLUME_H_
