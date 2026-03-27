#ifndef _OBB_H_
#define _OBB_H_

#include "BoundingVolume.h" 

class OBB : public BoundingVolume
{
public:
	OBB(Entity* entity);

private:
	Vector3 mMinExtent;
	Vector3 mMaxExtent;
	Vector3 mHalfExtents;

	std::vector<Vector3> axes;

	std::vector<Vector3> mCorners;
public:
	void UpdateBoundingVolume();

	Vector3& MinExtent() { return mMinExtent; };
	Vector3& MaxExtent() { return mMaxExtent; };



};



#endif // !_OBB_H_
