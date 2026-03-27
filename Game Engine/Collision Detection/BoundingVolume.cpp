#include "BoundingVolume.h"

BoundingVolume::BoundingVolume(Entity* entity) : mEntity(entity){}

Vector3 BoundingVolume::RelativeCenter()
{
	return mCenter + mEntity->Position();
}