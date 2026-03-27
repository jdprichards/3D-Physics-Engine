#include "IntegrationMethods.h"

#include "math.h"
#include "BoundingSphere.h"
#include "AABB.h"
#include "OBB.h"
#include "Plane.h"

void EulersMethod(Entity* entity, float frameTime)
{
	RigidBody* rigidBody = dynamic_cast<RigidBody*>(entity);

	if (rigidBody != nullptr)
	{
		rigidBody->Accelleration() = (rigidBody->ResultantForce() + rigidBody->FrictionForce()) * rigidBody->InverseMass();
		rigidBody->AngularAcceleration() = (rigidBody->ResultantTorque() + rigidBody->FrictionTorque()) * rigidBody->InverseInertiaTensor();

		rigidBody->Velocity() += rigidBody->Accelleration() * frameTime;
		rigidBody->AngularVelocity() += rigidBody->AngularAcceleration() * frameTime;

		if (rigidBody->CollisionType() == ECollisionType::BoundingSphere && rigidBody->Rolling())
		{
			rigidBody->Transform().Position() += (rigidBody->Velocity() + Cross(rigidBody->AngularVelocity() * rigidBody->GetBoundingVolume<BoundingSphere>()->Radius(), {0, 1, 0})) * frameTime;
		}
		else
		{
			rigidBody->Transform().Position() += rigidBody->Velocity() * frameTime;
		}

		Matrix4x4 matrixRotation = MatrixRotation(rigidBody->AngularVelocity() * frameTime);
		rigidBody->Transform(1) *= matrixRotation;
	}
}

void RungeKuttaMethod(Entity* entity, float frameTime)
{
	RigidBody* rigidBody = dynamic_cast<RigidBody*>(entity);

	if (rigidBody != nullptr)
	{
		float halfFrame = frameTime * 0.5f;

		rigidBody->Accelleration() = (rigidBody->ResultantForce() + rigidBody->FrictionForce()) * rigidBody->InverseMass();
		rigidBody->AngularAcceleration() = (rigidBody->ResultantTorque() + rigidBody->FrictionTorque()) * rigidBody->InverseInertiaTensor();

		Vector3 V_k1 = rigidBody->Accelleration() * frameTime;
		Vector3 V_k2 = rigidBody->Accelleration() + V_k1 * halfFrame;
		Vector3 V_k3 = rigidBody->Accelleration() + V_k2 * halfFrame;
		Vector3 V_k4 = rigidBody->Accelleration() + V_k3 * frameTime;

		Vector3 deltaVelocity = frameTime * (V_k1 + 2 * V_k2 + 2 * V_k3 + V_k4) / 6;

		// For Linear Velocity
		Vector3 VA_k1 = rigidBody->AngularAcceleration() * frameTime;
		Vector3 VA_k2 = rigidBody->AngularAcceleration() + VA_k1 * halfFrame;
		Vector3 VA_k3 = rigidBody->AngularAcceleration() * VA_k2 * halfFrame;
		Vector3 VA_k4 = rigidBody->AngularAcceleration() * VA_k3 * frameTime;

		// For Angular Velocity
		Vector3 deltaAngularVelocity = frameTime * (VA_k1 + 2 * VA_k2 + 2 * VA_k3 + VA_k4) / 6;

		Vector3 P_k1 = rigidBody->Velocity() * frameTime;
		Vector3 P_k2 = rigidBody->Velocity() + P_k1 * halfFrame;
		Vector3 P_k3 = rigidBody->Velocity() + P_k2 * halfFrame;
		Vector3 P_k4 = rigidBody->Velocity() + P_k3 * frameTime;

		Vector3 deltaPosition = frameTime * (P_k1 + 2 * P_k2 + 2 * P_k3 + P_k4) / 6;

		// For Position
		Vector3 R_k1 = rigidBody->AngularVelocity() * frameTime;
		Vector3 R_k2 = rigidBody->AngularVelocity() + R_k1 * halfFrame;
		Vector3 R_k3 = rigidBody->AngularVelocity() + R_k2 * halfFrame;
		Vector3 R_k4 = rigidBody->AngularVelocity() + R_k3 * frameTime;

		// For Rotation
		Vector3 deltaRotation = frameTime * (R_k1 + 2 * R_k2 + 2 * R_k3 + R_k4) / 6;

		// Updating Velocities and Transforms
		rigidBody->Velocity() += deltaVelocity;
		rigidBody->AngularVelocity() += deltaAngularVelocity;

		// For spheres only:
		if (rigidBody->CollisionType() == ECollisionType::BoundingSphere && rigidBody->Rolling())
		{
			rigidBody->Transform().Position() += (deltaPosition + Cross(deltaRotation * rigidBody->GetBoundingVolume<BoundingSphere>()->Radius(), { 0, 1, 0 }));
		}
		else
		{
			rigidBody->Transform().Position() += deltaPosition;
		}

		Matrix4x4 matrixRotation = MatrixRotation(deltaRotation);
		rigidBody->Transform(1) *= matrixRotation;
	}
}