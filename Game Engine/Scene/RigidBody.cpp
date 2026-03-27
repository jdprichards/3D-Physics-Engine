#include "RigidBody.h"

#include "BoundingSphere.h"
#include "AABB.h"
#include "OBB.h"
#include "Plane.h"

RigidBody::RigidBody(EntityTemplate& entityTemplate, EntityID UID, const Matrix4x4& transform, const std::string& name, const Vector3& velocity, const Vector3& angularVelocity)
	: Entity(entityTemplate, UID, transform, name)
{
	mEntityType = EEntityType::RigidBody;

	// Add Gravity
	AddForce(CenterOfMass(), Vector3(0.0f, -9.81f, 0.0f) * mMass);

	// Add "Push" Force
	//AddForce(mCenterOfMass + Vector3{ 0.0f, 0.0f, -1.0f }, { 0.0f, 500.0f, 1000.0f });

	mVelocity = velocity;
	//mVelocity = { 20.0f, 10.0f, 20.0f };
	mAngularVelocity = angularVelocity;
	//mAngularVelocity = { 1.0f, 0.5f, 1.0f };

}

bool RigidBody::Update(float frameTime)
{
	if (!Entity::Update(frameTime))
		return false;

	SimulatePhysics(frameTime);

	return true;
}

void RigidBody::AddForce(Vector3 position, Vector3 forceVec)
{
	SForce force;
	force.position = position;
	force.vector = forceVec;

	mForces.push_back(force);
}

void RigidBody::SimulatePhysics(float frameTime)
{
	//if (CollisionType() == ECollisionType::AABB)
	//	CreateAABB(this);

	//if (CollisionType() == ECollisionType::BoundingSphere)
	//	CreateSphereBB(this);

	CalculateResultantForce();
	CalculateResultantTorque();

	//EulersMethod(this, frameTime);
	RungeKuttaMethod(this, frameTime);

	FrictionForce() = { 0,0,0 };
	FrictionTorque() = { 0,0,0 };

	Rolling() = false;
}

// Moved to Entity.cpp (here for historical purposes)

//bool RigidBody::BoundingSphereCollision(float frameTime)
//{
//	float lowestY = Transform().Position().y - mBoundingSphereRadius;
//	if (lowestY < 0.0f)
//	{
//		return true;
//	}
//
//	return false;
//}

//bool RigidBody::AABBCollision(float frameTime)
//{
//	float lowestY = Transform().Position().y + mAABBBoundingMin.y;
//	if (lowestY < 0.0f)
//		return true;
//
//	return false;
//}

//float RigidBody::CalculateVolume()
//{
//	unsigned int polygonCount = Template().GetMesh().PolygonCount();
//	float volume = 0;
//
//	std::vector<Vector3> vertices = Template().GetMesh().Vertices();
//
//	for (int i = 0; i < polygonCount; i += 3)
//	{
//		float detJ = Dot(vertices[i], Cross(vertices[i + 1], vertices[i + 2]));
//		volume += 6 * detJ;
//	}
//
//	return volume;
//}

//float RigidBody::CalculateInertiaMomentX(std::vector<Vector3> polygon)
//{
//	float result =
//		Square(polygon[0].x) + polygon[1].x * polygon[2].x +
//		Square(polygon[1].x) + polygon[0].x * polygon[2].x +
//		Square(polygon[2].x) + polygon[0].x * polygon[1].x;
//	
//	return result;
//}

//float RigidBody::CalculateInertiaMomentY(std::vector<Vector3> polygon)
//{
//	float result =
//		Square(polygon[0].y) + polygon[1].y * polygon[2].y +
//		Square(polygon[1].y) + polygon[0].y * polygon[2].y +
//		Square(polygon[2].y) + polygon[0].y * polygon[1].y;
//
//	return result;
//}

//float RigidBody::CalculateInertiaMomentZ(std::vector<Vector3> polygon)
//{
//	float result =
//		Square(polygon[0].z) + polygon[1].z * polygon[2].z +
//		Square(polygon[1].z) + polygon[0].z * polygon[2].z +
//		Square(polygon[2].z) + polygon[0].z * polygon[1].z;
//
//	return result;
//}

//float RigidBody::CalculateInertiaProductXY(std::vector<Vector3> polygon)
//{
//	float result =
//		2 * polygon[0].x * polygon[0].y + polygon[1].x * polygon[2].y + polygon[2].x + polygon[1].y +
//		2 * polygon[1].x * polygon[1].y + polygon[0].x * polygon[2].y + polygon[2].x * polygon[0].y +
//		2 * polygon[2].x * polygon[2].y + polygon[0].x * polygon[1].y + polygon[1].x * polygon[0].y;
//
//	return result;
//}

//float RigidBody::CalculateInertiaProductXZ(std::vector<Vector3> polygon)
//{
//	float result =
//		2 * polygon[0].x * polygon[0].z + polygon[1].x * polygon[2].z + polygon[2].x + polygon[1].z +
//		2 * polygon[1].x * polygon[1].z + polygon[0].x * polygon[2].z + polygon[2].x * polygon[0].z +
//		2 * polygon[2].x * polygon[2].z + polygon[0].x * polygon[1].z + polygon[1].x * polygon[0].z;
//
//	return result;
//}

//float RigidBody::CalculateInertiaProductYZ(std::vector<Vector3> polygon)
//{
//	float result =
//		2 * polygon[0].y * polygon[0].z + polygon[1].y * polygon[2].z + polygon[2].y + polygon[1].z +
//		2 * polygon[1].y * polygon[1].z + polygon[0].y * polygon[2].z + polygon[2].y * polygon[0].z +
//		2 * polygon[2].y * polygon[2].z + polygon[0].y * polygon[1].z + polygon[1].y * polygon[0].z;
//
//	return result;
//}

//Matrix3x3 RigidBody::CalculateInertiaTensor()
//{	
//	unsigned int polygonCount = Template().GetMesh().PolygonCount();
//
//	float Ix = 0.0f;
//	float Iy = 0.0f;
//	float Iz = 0.0f;
//
//	float Ixy = 0.0f;
//	float Ixz = 0.0f;
//	float Iyz = 0.0f;
//
//	float mass = 0.0f;
//	
//	Vector3 centerOfMass = { 0, 0, 0 };
//	Vector3 centerMass = { 0,0,0 };
//	
//	std::vector<Vector3> polygonVectices;
//	std::vector<Vector3> vertices = Template().GetMesh().Vertices();
//	
//	int polygonsDone = 0;
//
//	for (int i = 0; i < polygonCount * 3; i += 3)
//	{
//		polygonVectices = std::vector({ vertices[i], vertices[i + 1], vertices[i + 2] });
//
//		float detJ = Dot(polygonVectices[0], Cross(polygonVectices[1], polygonVectices[2]));
//		
//		float polygonVolume = detJ / 6.0f;
//		float polygonMass = mDensity * polygonVolume;
//
//		Vector3 polyCenterOfMass = (polygonVectices[0] + polygonVectices[1] + polygonVectices[2]) / 4.0f;
//
//		 Ix	+= detJ * (CalculateInertiaMomentY(polygonVectices) + CalculateInertiaMomentZ(polygonVectices));
//		 Iy += detJ * (CalculateInertiaMomentX(polygonVectices) + CalculateInertiaMomentZ(polygonVectices));
//		 Iz += detJ * (CalculateInertiaMomentX(polygonVectices) + CalculateInertiaMomentY(polygonVectices));
//
//		 Ixy += detJ * (CalculateInertiaProductXY(polygonVectices));
//		 Ixz += detJ * (CalculateInertiaProductXZ(polygonVectices));
//		 Iyz += detJ * (CalculateInertiaProductYZ(polygonVectices));
//
//		 centerMass += polyCenterOfMass;
//		 centerOfMass += polyCenterOfMass * polygonMass;
//		 mass += polygonMass;
//
//		 polygonsDone++;
//	}
//
//	centerOfMass /= mass;
//	Ix = mDensity * Ix / 60.0f;
//	Iy = mDensity * Iy / 60.0f;
//	Iz = mDensity * Iz / 60.0f;
//
//	Ixy = mDensity * Ixy / 120.0f;
//	Ixz = mDensity * Ixz / 120.0f;
//	Iyz = mDensity * Iyz / 120.0f;
//
//	Matrix3x3 InertiaTensor =
//	{
//		Ix, -Ixy, -Ixz,
//		-Ixy, Iy, -Iyz,
//		-Ixz, -Iyz, Iz
//	};
//
//	mCenterOfMass = centerOfMass;
//	mMass = mass;
//	mVolume = mMass * mDensity;
//	mInverseMass = 1 / mass;
//
//
//	SForce gravity;
//	gravity.position = mCenterOfMass;
//	gravity.vector = { 0, -9.81, 0.0f };
//	gravity.vector *= mMass;
//
//	mForces.push_back(gravity);
//
//	SForce force;
//	force.position = mCenterOfMass + Vector3{0.0f, 1.0f, 0.0f};
//	force.vector = {0.0f, 0.0f, 1.0f};
//	mForces.push_back(force);
//
//	return InertiaTensor;
//}

//void RigidBody::CreateBoundingAABB()
//{
//	unsigned int polygonCount = Template().GetMesh().PolygonCount();
//
//	std::vector<Vector3> polygonVectices;
//	std::vector<Vector3> vertices = Template().GetMesh().ScaledVertices(Transforms());
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
//	mAABBBoundingMin = { minX, minY, minZ };
//	mAABBBoundingMax = { maxX, maxY, maxZ };
//}
//
//void RigidBody::CreateBoundingSphere()
//{
//	unsigned int polygonCount = Template().GetMesh().PolygonCount();
//
//	std::vector<Vector3> polygonVectices;
//	std::vector<Vector3> vertices = Template().GetMesh().ScaledVertices(Transforms());
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
//	mBoundingSphereCenter = { minX + (maxX - minX) / 2, minY + (maxY - minY) / 2, minZ + (maxZ - minZ) / 2 };
//	mBoundingSphereRadius = (std::max({ maxX - minX, maxY - minY, maxZ - minZ }) / 2);
//}

void RigidBody::CalculateResultantForce()
{
	Vector3 resultantForce = { 0.0f, 0.0f, 0.0f };

	for (auto& force : mForces)
	{
		resultantForce += force.vector;
	}

	mResulantForce = resultantForce;
}

void RigidBody::CalculateResultantTorque()
{
	Vector3 resultantTorque = { 0.0f, 0.0f, 0.0f };
	
	for (auto& force : mForces)
	{
		// Get distance of of rotation force to center of mass 

		Vector3 relativeForcePos = Transform().Position() + force.position;
		Vector3 relativeForceVec = force.vector;
		Vector3 r_vector = relativeForcePos - Transform().Position();

		Vector3 torque = Cross(r_vector, relativeForceVec);
		resultantTorque += torque;
	}

	mResulantTorque = resultantTorque;
}

void RigidBody::ApplyLinearImpulse(const Vector3& impulse)
{
	mVelocity += impulse * mInverseMass;
}

void RigidBody::ApplyAngularImpulse(const Vector3& impulseTorque)
{
	mAngularVelocity += impulseTorque * mInverseInertiaTensor;
}

void RigidBody::ApplyFrictionalForce(const Vector3& force)
{
	mFrictionForce += force;
}

void RigidBody::ApplyFrictionTorque(const Vector3& torque)
{
	mFrictionTorque += torque;
}