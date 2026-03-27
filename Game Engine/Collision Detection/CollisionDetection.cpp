#include "CollisionDetection.h"
#include "RigidBody.h"
#include "EntityManager.h"

#include <cassert>
// Following classes have been moved to their own class (see Collision Detection Folder)
// Kept here for future reference

//void CreateAABB(Entity* entity)
//{
//	std::vector<Vector3> vertices = entity->Template().GetMesh().ScaledVertices(entity->Transforms());
//	float maxX = -INFINITY;
//	float minX = INFINITY;
//	float maxY = -INFINITY;
//	float minY = INFINITY;
//	float maxZ = -INFINITY;
//	float minZ = INFINITY;
//
//	if (entity->GetName() == "Wall")
//		printf("Test");
//
//	for (auto vertex : vertices)
//	{
//		if (vertex.x > maxX)
//			maxX = vertex.x;
//		if (vertex.x < minX)
//			minX = vertex.x;
//
//		if (vertex.y > maxY)
//			maxY = vertex.y;
//		if (vertex.y < minY)
//			minY = vertex.y;
//
//		if (vertex.z > maxZ)
//			maxZ = vertex.z;
//		if (vertex.z < minZ)
//			minZ = vertex.z;
//	}
//
//	Vector3 AABBBoundingMin = { minX, minY, minZ };
//	Vector3 AABBBoundingMax = { maxX, maxY, maxZ };
//
//	entity->SetAABB(AABBBoundingMin, AABBBoundingMax);
//}
//
//void CreateOBB(Entity* entity)
//{
//	std::vector<Vector3> vertices = entity->Template().GetMesh().ScaledVertices(entity->Transforms());
//	vertices = entity->Template().GetMesh().Vertices();
//
//	Matrix3x3 covarianceMatrix = CalculateCovarienceMatrix(vertices, entity->CenterOfMass());
//
//	auto [eigenValues, eigenVectors] = EignenDecoposition(covarianceMatrix);
//
//	std::vector<Vector3> transformedVertices;
//
//	for (auto vertice : vertices)
//	{
//		transformedVertices.push_back(vertice * eigenVectors);
//	}
//
//	Vector3 minExtent = { INFINITY, INFINITY, INFINITY };
//	Vector3 maxExtent = { -INFINITY, -INFINITY, -INFINITY };
//
//	for (auto& vertice : transformedVertices)
//	{
//		if (vertice.x < minExtent.x)
//			minExtent.x = vertice.x;
//		if (vertice.y < minExtent.y)
//			minExtent.y = vertice.y;
//		if (vertice.z < minExtent.z)
//			minExtent.z = vertice.z;
//
//		if (vertice.x > minExtent.x)
//			maxExtent.x = vertice.x;
//		if (vertice.y > minExtent.y)
//			maxExtent.y = vertice.y;
//		if (vertice.z > minExtent.z)
//			maxExtent.z = vertice.z;
//	}
//
//	std::vector<Vector3> corners =
//	{
//		{minExtent.x, minExtent.y, minExtent.z},
//		{minExtent.x, minExtent.y, maxExtent.z},
//		{minExtent.x, maxExtent.y, minExtent.z},
//		{minExtent.x, maxExtent.y, maxExtent.z},
//		{maxExtent.x, minExtent.y, minExtent.z},
//		{maxExtent.x, minExtent.y, maxExtent.z},
//		{maxExtent.x, maxExtent.y, minExtent.z},
//		{maxExtent.x, maxExtent.y, maxExtent.z},
//	};
//
//	std::vector<Vector3> transformedCorners;
//	for (const auto& corner : corners)
//		transformedCorners.push_back(corner * eigenVectors + entity->CenterOfMass());
//
//	printf("Test");
//}
//
//void CreateSphereBB(Entity* entity)
//{
//	unsigned int polygonCount = entity->Template().GetMesh().PolygonCount();
//
//	std::vector<Vector3> polygonVectices;
//	std::vector<Vector3> vertices = entity->Template().GetMesh().ScaledVertices(entity->Transforms());
//
//	float maxX = -INFINITY;
//	float minX = INFINITY;
//	float maxY = -INFINITY;
//	float minY = INFINITY;
//	float maxZ = -INFINITY;
//	float minZ = INFINITY;
//
//	for (auto vertex : vertices)
//	{
//		if (vertex.x > maxX)
//			maxX = vertex.x;
//		if (vertex.x < minX)
//			minX = vertex.x;
//
//		if (vertex.y > maxY)
//			maxY = vertex.y;
//		if (vertex.y < minY)
//			minY = vertex.y;
//
//		if (vertex.z > maxZ)
//			maxZ = vertex.z;
//		if (vertex.z < minZ)
//			minZ = vertex.z;
//	}
//
//	Vector3 boundingSphereCenter = { minX + (maxX - minX) / 2, minY + (maxY - minY) / 2, minZ + (maxZ - minZ) / 2 };
//	float boundingSphereRadius = (std::max({ maxX - minX, maxY - minY, maxZ - minZ }) / 2);
//
//	entity->SetSphereBB(boundingSphereCenter, boundingSphereRadius);
//}


void CollisionDetection::CheckAllCollisions()
{
	// Check for collisions
	std::map<EntityID, std::unique_ptr<Entity>>& entities = gEntityManager->GetEntities();

	std::vector<CollisionInfo> collisionPairs;

	for (auto it1 = entities.begin(); it1 != entities.end();)
	{
		auto entity1 = it1->second.get();

		if ((entity1->EntityType() == EEntityType::Light) == false)
		{
			for (auto it2 = entities.begin(); it2 != entities.end();)
			{
				auto entity2 = it2->second.get();

				if (!(entity2->EntityType() == EEntityType::Light))
				{

					bool collision = false;

					// Checks that entity 1 and 2 aren't the same
					// Also checks to see if both entities are static (if they are they don't collide)S
					if (entity1 != entity2 && (entity1->EntityType() != EEntityType::Static || entity2->EntityType() != EEntityType::Static))
					{
						bool hasCollided = false;

						for (auto& collisionPair : collisionPairs)
						{
							if ((collisionPair.object1->GetID() == entity2->GetID() &&
								collisionPair.object2->GetID() == entity1->GetID()) ||
								(collisionPair.object1->GetID() == entity1->GetID() &&
								collisionPair.object2->GetID() == entity2->GetID()))
							{
								hasCollided = true;
								break;
							}
						}

						if (!hasCollided)
						{
							CollisionInfo collisionInfo;
							collision = CheckCollision(entity1, entity2, collisionInfo);
							
							if (collision)
							{
								collisionPairs.push_back({ collisionInfo });

							}
						}
					}
				}

				it2++;
			}
		}

		it1++;
	}

	for (auto& collisionPair : collisionPairs)
		// If objects have collided with each other
		// If they have don't check for another collision this frame
	{
		if (collisionPair.object1->EntityType() != EEntityType::Static || collisionPair.object2->EntityType() != EEntityType::Static)
			ResolveCollision(collisionPair);

	}
}

bool CollisionDetection::CheckCollision(Entity* entity1, Entity* entity2, CollisionInfo& collisionInfo)
{
	// If one of the entities don't have collision enabled - don't need to check for collisions
	if (entity1 != nullptr && entity2 != nullptr)
	{
		if (entity1->GetBoundingVolume<BoundingVolume>() == nullptr || entity2->GetBoundingVolume<BoundingVolume>() == nullptr)
			return false;

		bool collision = false;

		// Alogorithms for same collision types
		if (entity1->CollisionType() == ECollisionType::AABB && entity2->CollisionType() == ECollisionType::AABB)
		{
			collision = AABB2AABB_Collision(entity1->GetBoundingVolume<AABB>(), entity2->GetBoundingVolume<AABB>(), collisionInfo);
			if (collision)
			{
				collisionInfo.object1 = entity1;
				collisionInfo.object2 = entity2;
				return true;
			}
		}

		if (entity1->CollisionType() == ECollisionType::BoundingSphere && entity2->CollisionType() == ECollisionType::BoundingSphere)
		{
			collision = Sphere2Sphere_Collision(entity1->GetBoundingVolume<BoundingSphere>(), entity2->GetBoundingVolume<BoundingSphere>(), collisionInfo);
			if (collision)
			{
				collisionInfo.object1 = entity1;
				collisionInfo.object2 = entity2;
				return true;
			}
		}

		/*if (entity1->CollisionType() == ECollisionType::OBB && entity2->CollisionType() == ECollisionType::OBB)
		{
			collision = OBB2OBB_Collision(entity1->GetBoundingVolume<OBB>(), entity2->GetBoundingVolume<OBB>(), collisionInfo);
			if (collision)
			{
				collisionInfo.object1 = entity1;
				collisionInfo.object2 = entity2;
			}
		}*/

		// If collision types are different, the order that they're passed to the function depends on their specific collision type (AABB goes first)

		// AABB - Sphere Collision Detection
		if ((entity1->CollisionType() == ECollisionType::AABB && entity2->CollisionType() == ECollisionType::BoundingSphere))
		{
			collision = AABB2Sphere_Collision(entity1->GetBoundingVolume<AABB>(), entity2->GetBoundingVolume<BoundingSphere>(), collisionInfo);
			if (collision)
			{
				collisionInfo.object1 = entity1;
				collisionInfo.object2 = entity2;
				return true;
			}

		}
		else if ((entity1->CollisionType() == ECollisionType::BoundingSphere && entity2->CollisionType() == ECollisionType::AABB))
		{
			collision = AABB2Sphere_Collision(entity2->GetBoundingVolume<AABB>(), entity1->GetBoundingVolume<BoundingSphere>(), collisionInfo);
			if (collision)
			{
				collisionInfo.object1 = entity2;
				collisionInfo.object2 = entity1;
				return true;
			}
		}
		// AABB - Plane Collision Detection
		if (entity1->CollisionType() == ECollisionType::Plane && entity2->CollisionType() == ECollisionType::AABB)
		{
			collision = AABBPlane_Collision(entity2->GetBoundingVolume<AABB>(), entity1->GetBoundingVolume<Plane>(), collisionInfo);
			if (collision)
			{
				collisionInfo.object1 = entity2;
				collisionInfo.object2 = entity1;
				return true;
			}
		}

		else if (entity1->CollisionType() == ECollisionType::AABB && entity2->CollisionType() == ECollisionType::Plane)
		{
			collision = AABBPlane_Collision(entity1->GetBoundingVolume<AABB>(), entity2->GetBoundingVolume<Plane>(), collisionInfo);
			if (collision)
			{
				collisionInfo.object1 = entity2;
				collisionInfo.object2 = entity1;
				return true;
			}
		}
		// BoundingSphere - Plane Collision Detection
		if ((entity1->CollisionType() == ECollisionType::BoundingSphere && entity2->CollisionType() == ECollisionType::Plane))
		{
			collision = Sphere2Plane_Collision(entity1->GetBoundingVolume<BoundingSphere>(), entity2->GetBoundingVolume<Plane>(), collisionInfo);
			if (collision)
			{
				collisionInfo.object1 = entity1;
				collisionInfo.object2 = entity2;
				return true;
			}
		}
		else if ((entity1->CollisionType() == ECollisionType::Plane && entity2->CollisionType() == ECollisionType::BoundingSphere))
		{
			collision = Sphere2Plane_Collision(entity2->GetBoundingVolume<BoundingSphere>(), entity1->GetBoundingVolume<Plane>(), collisionInfo);
			if (collision)
			{
				collisionInfo.object1 = entity2;
				collisionInfo.object2 = entity1;
				return true;

			}
		}
		if (collision)
		{
			return true;
		}
	}
	
	return false;
}

Vector3 CollisionDetection::ClosestPoint_AABB2Sphere(AABB* aabb, BoundingSphere* sphere)
{
	Vector3 result;

	Vector3 minAABB = aabb->MinPoint();
	Vector3 maxAABB = aabb->MaxPoint();
	Vector3 center = sphere->RelativeCenter();

	// Clamping closest point to be inside object

	// For X

	if (center.x > maxAABB.x)
		result.x = maxAABB.x;
	else if (center.x < minAABB.x)
		result.x = minAABB.x;
	else
		result.x = center.x;

	// For Y
	if (center.y > maxAABB.y)
		result.y = maxAABB.y;
	else if (center.y < minAABB.y)
		result.y = minAABB.y;
	else
		result.y = center.y;

	// For Z
	if (center.z > maxAABB.z)
		result.z = maxAABB.z;
	else if (center.z < minAABB.z)
		result.z = minAABB.z;
	else
		result.z = center.z;

	return result;
}

// Axis-Aligned Bounding Box to Axis-Aligned Bounding Box Collsion
bool CollisionDetection::AABB2AABB_Collision(AABB* aabb1, AABB* aabb2, CollisionInfo& collisionInfo)
{
	Vector3 min1, min2, max1, max2;

	min1 = aabb1->MinPoint();
	min2 = aabb2->MinPoint();

	max1 = aabb1->MaxPoint();
	max2 = aabb2->MaxPoint();

	bool collision =  
		((min1.x <= max2.x && max1.x >= min2.x) &&
		(min1.y <= max2.y && max1.y >= min2.y) &&
		(min1.z <= max2.z && max1.z >= min2.z));

	if (collision)
	{
		const Vector3 faces[6] =
		{
			Vector3{-1, 0, 0}, Vector3{1, 0, 0},
			Vector3{0, -1, 0}, Vector3{0, 1, 0},
			Vector3{0, 0, -1}, Vector3{0, 0, 1}
		};

		float distancesBetween[6] =
		{
			(max1.x - min2.x),
			(max2.x - min1.x),
			(max1.y - min2.y),
			(max2.y - min1.y),
			(max1.z - min2.z),
			(max2.z - min1.z)
		};

		float penetration = INFINITY;
		Vector3 bestAxis;

		for (int i = 0; i < 6; i++)
		{
			if (distancesBetween[i] < penetration)
			{
				penetration = distancesBetween[i];
				bestAxis = faces[i];
			}
		}

		// Overlapping region
		Vector3 overlapMin = { std::max(min1.x, min2.x), std::max(min1.y, min2.y), std::max(min1.z, min2.z) };
		Vector3 overlapMax = { std::min(max1.x, max2.x), std::min(max1.y, max2.y), std::min(max1.z, max2.z) };

		// Center of overlapping region
		Vector3 centerOfOverlap = (overlapMin + overlapMax) / 2.0f;

		collisionInfo = { nullptr, nullptr, bestAxis, penetration, centerOfOverlap };

		return true;
	}

	return false;
}

// Sphere to Sphere Collision
bool CollisionDetection::Sphere2Sphere_Collision(BoundingSphere* sphere1, BoundingSphere* sphere2, CollisionInfo& collisionInfo)
{
	Vector3 vectorBetween = sphere1->RelativeCenter() - sphere2->RelativeCenter();
	float distanceBetween = vectorBetween.Length();

	float sphere2sphre = sphere1->Radius() + sphere2->Radius();
	if (distanceBetween < sphere1->Radius() + sphere2->Radius())
	{
		Vector3 normal = Normalise(vectorBetween);
		float penetration = (sphere2sphre) - distanceBetween;
		Vector3 contractPoint = sphere2->RelativeCenter() - sphere2->Radius() * normal;

		collisionInfo = { nullptr, nullptr, normal , penetration, contractPoint };

		return true;

	}
	return false;
}

// OBB - OBB collision using SAT ( Seperating Axis Theorm )
bool CollisionDetection::OBB2OBB_Collision(OBB* obb1, OBB* obb2, CollisionInfo& collisionInfo)
{
	std::vector<Vector3> axes;
	std::vector<Vector3> axes1;
	std::vector<Vector3> axes2;

	for (auto& axis : axes1)
		axes.push_back(axis);
	for (auto& axis : axes2)
		axes.push_back(axis);

	for (auto& axis1 : axes1)
		for (auto& axis2 : axes2)
			axes.push_back(Cross(axis1, axis2));
	return false;
}

// Axis-Aligned Bounding Box to Sphere Collision
// If collision types are different, the order that they're passed to the function depends on their specific collision type (AABB goes first)
bool CollisionDetection::AABB2Sphere_Collision(AABB* aabb, BoundingSphere* sphere, CollisionInfo& collisionInfo)
{
	Vector3 closestPoint = ClosestPoint_AABB2Sphere(aabb, sphere);
	Vector3 vectorBetween = closestPoint - sphere->RelativeCenter();

	float distanceSquared = vectorBetween.LengthSq();
	float radiusSquared = Square(sphere->Radius());

	bool collision = distanceSquared < radiusSquared;

	if (collision)
	{
		float penetration = sphere->Radius() - vectorBetween.Length();
		Vector3 normal = Normalise(vectorBetween);

		collisionInfo = { nullptr, nullptr, normal, penetration, closestPoint };
		return true;
	}

	return false;
}

bool CollisionDetection::ABB2OBB_Collision(AABB* aabb, OBB* obb, CollisionInfo& collisionInfo)
{
	return false;
}


float CollisionDetection::PointPlaneDistance(Vector3 point, Vector4 planeEquation)
{
	return planeEquation.x * point.x + planeEquation.y * point.y + planeEquation.z * point.z + planeEquation.w;
}

bool CollisionDetection::AABBPlane_Collision(AABB* aabb, Plane* plane, CollisionInfo& collisionInfo)
{
	Vector3 normal = plane->Normal();

	Vector3 minAABB = aabb->MinPoint();
	Vector3 maxAABB = aabb->MaxPoint();

	float projectionDistance = Dot((maxAABB - minAABB / 2) - aabb->Center(), normal);

	float distanceAABBPlane = Dot(normal, aabb->Center() - plane->Center());

	// Collision occurs
	if (std::abs(distanceAABBPlane) <= projectionDistance)
	{
		Vector3 bestAxis;
		float penetraion;

		Vector3 collisionPoint;

		std::vector<std::pair<Vector3, Vector3>> edges =
		{
			{{minAABB.x, maxAABB.y, maxAABB.z}, {maxAABB.x, maxAABB.y, maxAABB.z}},
			{{minAABB.x, maxAABB.y, maxAABB.z}, {minAABB.x, maxAABB.y, minAABB.z}},
			{{minAABB.x, maxAABB.y, maxAABB.z}, {minAABB.x, minAABB.y, maxAABB.z}},

			{{maxAABB.x, maxAABB.y, maxAABB.z}, {maxAABB.x, maxAABB.y, minAABB.z}},
			{{maxAABB.x, maxAABB.y, maxAABB.z}, {maxAABB.x, minAABB.y, maxAABB.z}},

			{{minAABB.x, maxAABB.y, minAABB.z}, {maxAABB.x, maxAABB.y, minAABB.z}},
			{{minAABB.x, maxAABB.y, minAABB.z}, {minAABB.x, minAABB.y, minAABB.z}},

			{{maxAABB.x, maxAABB.y, minAABB.z}, {maxAABB.x, minAABB.y, minAABB.z}},

			{{minAABB.x, minAABB.y, maxAABB.z}, {maxAABB.x, minAABB.y, minAABB.z}},
			{{minAABB.x, minAABB.y, maxAABB.z}, {minAABB.x, minAABB.y, minAABB.z}},

			{{maxAABB.x, minAABB.y, maxAABB.z}, {maxAABB.x, minAABB.y, minAABB.z}},

			{{minAABB.x, minAABB.y, minAABB.z}, {maxAABB.x, minAABB.y, minAABB.z}}
		};

		float d = -Dot(normal, plane->PlanePoint());
		Vector4 plane = { normal.x, normal.y, normal.z, d };

		std::vector<Vector3> intersectPoints;
		for (auto& combination : edges)
		{
			Vector3 edgeStart = combination.first;
			Vector3 edgeEnd = combination.second;

			Vector3 direction = edgeEnd - edgeStart;
			float denominator = plane.x * direction.x + plane.y * direction.y + plane.z * direction.z;

			if (IsZero(denominator))
				continue;
			
			float t = -(plane.x * edgeStart.x + plane.y * edgeStart.y + plane.z * edgeStart.z + d) / denominator;
				if (isinf(t)) // If plane and edge are parallel
					continue;

			Vector3 intersectPoint = edgeStart + t * direction;
					
			intersectPoints.push_back(intersectPoint);
		}

		Vector3 averageContractPoint = { 0,0,0 };

		if (intersectPoints.size() > 0)
		{
			for (auto& point : intersectPoints)
				averageContractPoint += point;
			averageContractPoint /= intersectPoints.size();
		}
		else
			return false;

		float penetration = projectionDistance - std::abs(distanceAABBPlane);

		collisionInfo = { nullptr, nullptr, normal, penetration, averageContractPoint };
		

		return true;
	}
	
	return false;
}

bool CollisionDetection::Sphere2OBB_Collision(BoundingSphere* sphere, OBB* obb, CollisionInfo& collisionInfo)
{
	return false;
}

// Sphere to Plane Collision
bool CollisionDetection::Sphere2Plane_Collision(BoundingSphere* sphere, Plane* plane, CollisionInfo& collisionInfo)
{
	Vector3 normal = plane->Normal();

	Vector3 vectorBetween = sphere->RelativeCenter() - plane->RelativeCenter();

	float distance = abs(Dot(vectorBetween, normal));

	float projectionDistance = Dot(sphere->RelativeCenter(), normal);

	// Collision occurs
	if (distance <= sphere->Radius())
	{
		Vector3 bestAxis;
		float penetraion;

		Vector3 collisionPoint;

		float d = -Dot(normal, plane->PlanePoint());
		Vector4 plane = { normal.x, normal.y, normal.z, d };

		penetraion = sphere->Radius() - distance;

		collisionPoint = sphere->RelativeCenter() - normal * distance;

		collisionInfo = { nullptr, nullptr, normal, penetraion, collisionPoint };

		return true;
	}


	return false;
}

bool CollisionDetection::OBB2Plane_Collision(OBB* obb, Plane* plane)
{
	return false;
}

void CollisionDetection::ResolveCollision(CollisionInfo& collisionInfo)
{
	// Extraction Key information from the collision Information Structure
	Entity* object1 = collisionInfo.object1;
	Entity* object2 = collisionInfo.object2;

	float restitution = object1->CoefficientRestitution() * object2->CoefficientRestitution();

	Vector3 relativePositionA = object1->Position() - collisionInfo.collisionPoint;
	Vector3 relativePositionB = object2->Position() - collisionInfo.collisionPoint;

	// if object 1 or 2 is a plane set the relative position of that object to be the collision point
	// Stops the relative position from being a huge vector when far away from the origin
	// Which majorly effects the rotational effects of the collision.
	if (object1->CollisionType() == ECollisionType::Plane)
		relativePositionA = collisionInfo.collisionPoint;
	if (object2->CollisionType() == ECollisionType::Plane)
		relativePositionB = collisionInfo.collisionPoint;

	float inverseMassA = collisionInfo.object1->InverseMass();
	float inverseMassB = collisionInfo.object2->InverseMass();

	Matrix3x3 inverseIa = collisionInfo.object1->InverseInertiaTensor();
	Matrix3x3 inverseIb = collisionInfo.object2->InverseInertiaTensor();

	Vector3 collisionNormal = collisionInfo.bestAxis;
	float penetration = collisionInfo.penetraion;

	// Check to see if collision object is a rigid body
	// returns a nullptr if its not a rigid body or inhertis from the RigidBody class
	RigidBody* rigidBodyObject1 = dynamic_cast<RigidBody*>(collisionInfo.object1);
	RigidBody* rigidBodyObject2 = dynamic_cast<RigidBody*>(collisionInfo.object2);

	// To conserve momentum
	// Static objects don't move so to stop objects from coming to a halt (much which than normal)
	// Mass and Inertia of static object is set to 0
	// So only the mass and Inertia of rigid body objects and collision normals determine its impulse
	if (rigidBodyObject1 == nullptr)
	{
		inverseMassA = 0;
		inverseIa =
		{
			0,0,0,
			0,0,0,
			0,0,0
		};
	}
	if (rigidBodyObject2 == nullptr)
	{
		inverseMassB = 0;
		inverseIb =
		{
			0,0,0,
			0,0,0,
			0,0,0
		};
	}

	Vector3 relativeVelocity = (object2->Velocity() + Cross(object2->AngularVelocity(), relativePositionB)) - (object1->Velocity() + Cross(object1->AngularVelocity(), relativePositionA));

	float relativeNormal = Dot(relativeVelocity, collisionNormal);

	float numerator = -(1.0f + restitution) * relativeNormal;

	float totalInverseMass = inverseMassA + inverseMassB;

	Vector3 inertiaA = Cross(inverseIa * Cross(relativePositionA, collisionNormal), relativePositionA);
	Vector3 inertiaB = Cross(inverseIb * Cross(relativePositionB, collisionNormal), relativePositionB);
	float dotInertia = Dot(inertiaA, collisionNormal) + Dot(inertiaB, collisionNormal);

	float denominator = totalInverseMass + dotInertia;

	float impulseScaler = numerator / denominator;
	Vector3 impulseVector = impulseScaler * collisionNormal;

	float frictionCoef = object1->FrictionCoefficient() * object2->FrictionCoefficient();

	// If only one collision body is a RigidBody
	// Physical effects only apply to the rigid boody 
	if (rigidBodyObject1 == nullptr || rigidBodyObject2 == nullptr)
	{
		// Check to see if collision body 1 or 2 is a rigid body
		if (rigidBodyObject1 != nullptr)
		{
			Vector3 rotationImpulse = Cross(impulseVector, relativePositionA);

			// Impulse due to collision
			rigidBodyObject1->ApplyLinearImpulse(-impulseVector);
			rigidBodyObject1->ApplyAngularImpulse(-rotationImpulse);
			
			// Force due to friction
			Vector3 tangent = relativeVelocity - collisionNormal * Dot(relativeVelocity, collisionNormal);
			tangent = Normalise(tangent);

			Vector3 resulantForce = rigidBodyObject1->ResultantForce();
			float angle = ToRadians(AngleBetween(resulantForce, collisionNormal));

			float normalMangitude = (resulantForce * std::cos(angle)).Length();
			Vector3 frictionForce = tangent * normalMangitude * frictionCoef;

			Vector3 relativeTangent = Cross(relativePositionA, tangent);
			Vector3 frictionTorque = relativeTangent * normalMangitude * frictionCoef;
			frictionTorque = -(relativeTangent * normalMangitude * frictionCoef).Length() * Normalise(rigidBodyObject1->AngularVelocity());

			// Dynamic Friction can only be applied when object is moving
			if (!IsZero(rigidBodyObject1->Velocity().Length()) && !IsZero(frictionForce.Length()))
			{
				rigidBodyObject1->ApplyFrictionalForce(frictionForce);
			}
			if (!IsZero(rigidBodyObject1->AngularVelocity().Length()) && !IsZero(frictionTorque.Length()))
			{
				rigidBodyObject1->ApplyFrictionTorque(frictionTorque);
			}
			// Move penetration depth ( avoid objects still intersecting next frame )
			rigidBodyObject1->Position() += penetration * collisionNormal;

			if (IsZero(rigidBodyObject1->AngularVelocity().Length()))
			{
				rigidBodyObject1->Rolling() = true;
			}
		}

		if (rigidBodyObject2 != nullptr)
		{
			Vector3 rotationImpulse = Cross(impulseVector, relativePositionB);

			// Impulse due to collision
			rigidBodyObject2->ApplyLinearImpulse(impulseVector);
			rigidBodyObject2->ApplyAngularImpulse(rotationImpulse);

			// Force due to friction
			Vector3 tangent = relativeVelocity - collisionNormal * Dot(relativeVelocity, collisionNormal);
			tangent = Normalise(tangent);

			Vector3 resulantForce = rigidBodyObject2->ResultantForce();
			float angle = ToRadians(AngleBetween(resulantForce, collisionNormal));

			float normalMangitude = (resulantForce * std::cos(angle)).Length();
			Vector3 frictionForce = tangent * normalMangitude * frictionCoef;

			Vector3 relativeTangent = Cross(relativePositionB, tangent);
			Vector3 frictionTorque = Cross(relativePositionB, tangent) * normalMangitude * frictionCoef;
			frictionTorque = -(relativeTangent * normalMangitude * frictionCoef).Length() * Normalise(rigidBodyObject2->AngularVelocity());

			// Dynamic Friction can only be applied when object is moving
			if (!IsZero(rigidBodyObject2->Velocity().Length()) && !IsZero(frictionForce.Length()))
			{
				rigidBodyObject2->ApplyFrictionalForce(-frictionForce);
			}
			if (!IsZero(rigidBodyObject2->AngularVelocity().Length()) && !IsZero(frictionTorque.Length()))
			{
				rigidBodyObject2->ApplyFrictionTorque(frictionTorque);
			}

			// Move penetration depth ( avoid objects still intersecting next frame )
			rigidBodyObject2->Position() -= penetration * collisionNormal;

			if (IsZero(rigidBodyObject2->AngularVelocity().Length()))
			{
				rigidBodyObject2->Rolling() = true;
			}
		}
	}

	// If both collision bodies is a RigidBody
	// Physical effects apply to both bodies
	else
	{
		// Impulse due to collision
		Vector3 rotationImpulseA = Cross(impulseVector, relativePositionA);
		Vector3 rotationImpulseB = Cross(impulseVector, relativePositionB);

		rotationImpulseA = Cross(impulseVector, collisionNormal * relativePositionA);
		rotationImpulseB = Cross(impulseVector, collisionNormal * relativePositionB);

		// Body 1
		rigidBodyObject1->ApplyLinearImpulse(-impulseVector);
		rigidBodyObject1->ApplyAngularImpulse(-rotationImpulseA);

		// Body 2
		rigidBodyObject2->ApplyLinearImpulse(impulseVector);
		rigidBodyObject2->ApplyAngularImpulse(rotationImpulseB);

		// Deceleration force due to friction
		Vector3 tangent = relativeVelocity - collisionNormal * Dot(relativeVelocity, collisionNormal);

		tangent = Normalise(tangent);

		// Body 1
		Vector3 resulantForce = rigidBodyObject1->ResultantForce();
		float angle = ToRadians(AngleBetween(resulantForce, collisionNormal));

		float normalMangitude = (resulantForce * std::cos(angle)).Length();
		Vector3 frictionForce = tangent * normalMangitude * frictionCoef;

		Vector3 relativeTangentA = Cross(relativePositionA, tangent);
		Vector3 frictionTorque = relativeTangentA * normalMangitude * frictionCoef;
		frictionTorque = -(relativeTangentA * normalMangitude * frictionCoef).Length() * Normalise(rigidBodyObject1->AngularVelocity());

		// Dynamic Friction can only be applied when object is moving
		if (!IsZero(rigidBodyObject1->Velocity().Length()) && !IsZero(frictionForce.Length()))
		{
			rigidBodyObject1->ApplyFrictionalForce(frictionForce);
		}
		if (!IsZero(rigidBodyObject1->AngularVelocity().Length()) && !IsZero(frictionTorque.Length()))
		{
			rigidBodyObject1->ApplyFrictionTorque(frictionTorque);
		}
		// Body 2
		resulantForce = rigidBodyObject2->ResultantForce();
		angle = ToRadians(AngleBetween(resulantForce, collisionNormal));

		normalMangitude = (resulantForce * std::cos(angle)).Length();
		frictionForce = -tangent * normalMangitude * frictionCoef;

		Vector3 relativeTangentB = Cross(relativePositionB, tangent);
		frictionTorque = relativeTangentB * normalMangitude * frictionCoef;
		frictionTorque = -(relativeTangentB * normalMangitude * frictionCoef).Length() * Normalise(rigidBodyObject2->AngularVelocity());

		// Dynamic Friction can only be applied when object is moving
		if (!IsZero(rigidBodyObject2->Velocity().Length()) && !IsZero(frictionForce.Length()))
		{
			rigidBodyObject2->ApplyFrictionalForce(frictionForce);
		}
		if (!IsZero(rigidBodyObject2->AngularVelocity().Length()) && !IsZero(frictionTorque.Length()))
		{
			rigidBodyObject2->ApplyFrictionTorque(frictionTorque);
		}
		// Move penetration depth ( avoid objects still intersecting next frame ) - half so both objects are moved the same distance away
		rigidBodyObject1->Position() += penetration * collisionNormal / 2;
		rigidBodyObject2->Position() -= penetration * collisionNormal / 2;

		// Sets rolling to true for the next frame (is set back to zero at end of RigidBody::Update() so it the object only rolls while "colliding" with something)
		if (IsZero(rigidBodyObject1->AngularVelocity().Length()))
			rigidBodyObject1->Rolling() = true;

		if (IsZero(rigidBodyObject2->AngularVelocity().Length()))
			rigidBodyObject2->Rolling() = true;
	}
}