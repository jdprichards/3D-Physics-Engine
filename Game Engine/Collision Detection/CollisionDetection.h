#ifndef _COLLISION_DETECTION_H_
#define _COLLISION_DETECTION_H_

#include "math.h"
#include "Entity.h"

#include "BoundingSphere.h"
#include "AABB.h"
#include "OBB.h"
#include "Plane.h"
#include "CollisionTypes.h"

class Entity;

struct CollisionInfo
{
	Entity* object1;
	Entity* object2;

	Vector3 bestAxis;
	float penetraion;

	Vector3 collisionPoint;
};

class CollisionDetection
{

public:
	CollisionDetection() {};

public:

	void CheckAllCollisions();
	// Overall function for checking collisions, specific collision algorithms chosen from types of collisions in entities
	bool CheckCollision(Entity* entity1, Entity* entity2, CollisionInfo& collisionInfo);

private:
	//void CreateAABB(Entity* entity);
	//void CreateOBB(Entity* entity);
	//void CreateSphereBB(Entity* entity);

	//Matrix3x3 CalculateCovarienceMatrix(std::vector<Vector3> vertices, Vector3 center);
	//
	//// A Matrix factorization technique to Decompose a Matrix (Q), into a Product of an Orthoginal Matrix (O)
	//// and a Upper Triangle matrix (R).
	//// Where:
	//// - Transpose(Q)*Q is an Identity Matrix
	//// - R_(i,j) = 0 (For i > j): R_(row, column)
	//template<typename T> void QR_Decomposition(const Matrix3x3T<T>& M, Matrix3x3T<T>& Q, Matrix3x3T<T>& R);
	//template<typename T> std::pair<Vector3T<T>, Matrix3x3T<T>> EignenDecoposition(Matrix3x3T<T> m);

	float PointPlaneDistance(Vector3 point, Vector4 planeEquation);
	Vector3 ClosestPoint_AABB2Sphere(AABB* aabb, BoundingSphere* sphere);

	// Functions for indivdual types of collisions. (AABBs, Spheres, Planes, etc.)
	bool AABB2AABB_Collision(AABB* aabb1, AABB* aabb2, CollisionInfo& collisionInfo);
	bool Sphere2Sphere_Collision(BoundingSphere* sphere1, BoundingSphere* sphere2, CollisionInfo& collisionInfo);
	bool OBB2OBB_Collision(OBB* obb1, OBB* obb2, CollisionInfo& collisionInfo);

	bool AABBPlane_Collision(AABB* aabb, Plane* plane, CollisionInfo& collisionInfo);
	bool AABB2Sphere_Collision(AABB* aabb, BoundingSphere* sphere, CollisionInfo& collisionInfo);
	bool ABB2OBB_Collision(AABB* aabb, OBB* obb, CollisionInfo& collisionInfo);

	bool Sphere2OBB_Collision(BoundingSphere* sphere, OBB* obb, CollisionInfo& collisionInfo);
	bool Sphere2Plane_Collision(BoundingSphere* sphere, Plane* plane, CollisionInfo& collisionInfo);

	bool OBB2Plane_Collision(OBB* obb, Plane* plane);

	void ResolveCollision(CollisionInfo& collisionInfo);

};

#endif // !_COLLISION_DETECTION_H_
