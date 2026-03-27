#ifndef _RIGID_BODY_H_
#define _RIGID_BODY_H_

#include "Entity.h"
#include "IntegrationMethods.h"


class RigidBody : public Entity
{
	struct SForce
	{
		Vector3 position;
		Vector3 vector;
	};

public:
	RigidBody(EntityTemplate& entityTemplate, EntityID UID, const Matrix4x4& transform = Matrix4x4::Identity, const std::string& name = "", const Vector3& velocity = Vector3{ 0,0,0 }, const Vector3& angularVelocity = Vector3{ 0,0,0 });

	bool Update(float frameTime) override;

	void AddForce(Vector3 position, Vector3 force);

	void SimulatePhysics(float frameTime);

	//bool BoundingSphereCollision(float frameTime);
	//bool AABBCollision(float frameTime);

	void ApplyLinearImpulse(const Vector3& force);
	void ApplyAngularImpulse(const Vector3& torque);

	void ApplyFrictionalForce(const Vector3& force);
	void ApplyFrictionTorque(const Vector3& torque);

	//float CalculateVolume();

	//float CalculateInertiaMomentX(std::vector<Vector3> vertex); // Axis to caclulate moment of inertia along
	//float CalculateInertiaMomentY(std::vector<Vector3> vertex);
	//float CalculateInertiaMomentZ(std::vector<Vector3> vertex);

	//float CalculateInertiaProductYZ(std::vector<Vector3> vertex); // Axes to calculate moment of inertia along (order matters)
	//float CalculateInertiaProductXZ(std::vector<Vector3> vertex);
	//float CalculateInertiaProductXY(std::vector<Vector3> vertex);

	Vector3& Accelleration() { return mAcceleration; }
	Vector3& AngularAcceleration() { return mAngularAcceleration; }

	Vector3& ResultantForce() { return mResulantForce; }
	Vector3& ResultantTorque() { return mResulantTorque; }

	Vector3& FrictionForce() { return mFrictionForce; }
	Vector3& FrictionTorque() { return mFrictionTorque; }
	//Vector3& ABBBoundingBoxMin() { return mAABBBoundingMin; }
	//Vector3& ABBBoundingBoxMax() { return mAABBBoundingMax; }

	//Vector3& SphereBoundingCenter() { return mBoundingSphereCenter; }
	//float& SphereBoundingRadius() { return mBoundingSphereRadius; }

	bool& Rolling() { return mRolling; }

	void CalculateResultantForce();
	void CalculateResultantTorque();

private:

	Vector3 mAcceleration;
	Vector3 mAngularAcceleration;

	Vector3 mGravity = { 0, -9.81f, 0 };

	std::vector<SForce> mForces;
	std::vector<Vector3> mTorques;

	Vector3 mFrictionForce = { 0,0,0 };
	Vector3 mFrictionTorque = { 0,0,0 };

	Vector3 mResulantForce = { 0,0,0 };
	Vector3 mResulantTorque = { 0,0,0 };

	bool mRolling = false;
};

#endif // !_RIGID_BODY_H_