#ifndef _ENGINE_H_INCLUDED_
#define _ENGINE_H_INCLUDED_

#include "imgui.h"
#include "Scene.h"
#include "EntityManager.h"
#include "SceneGlobals.h"
#include "CollisionDetection.h"

class EngineGUI
{
public:
	EngineGUI() {}

	void ShowWindow();
	void ShowDemoWindow1(float frameTime);
	void ShowDemoWindow2(float frameTime);


private:

// Demo 1 variables
	float mDemo1SpawnRate = 2.0f;
	float mDemo1SpawnTime = 0.0f;
	float mDemo1LifeTime = 50.0f;

	Vector3 mDemo1SpawnPositionMin = { -25.0f, 0.0f, -25.0f };
	Vector3 mDemo1SpawnPositionMax = { 25.0f, 50.0f, 25.0f };

	Vector3 mDemo1SpawnRotationMin = { ToRadians(-180.0f), ToRadians(-180.0f), ToRadians(-180.0f)};
	Vector3 mDemo1SpawnRotationMax = { ToRadians(180.0f), ToRadians(180.0f), ToRadians(180.0f) };

	float mDemo1SpawnScaleMin = 0.25f;
	float mDemo1SpawnScaleMax = 1.0f;

	Vector3 mDemo1SpawnVelocityMin = { -25.0f, -5.0f, -25.0f };
	Vector3 mDemo1SpawnVelocityMax = { 25.0f, 12.5f, 25.0f };

	Vector3 mDemo1SpawnAngularVelocityMin = { ToRadians(-90.0f), ToRadians(-90.0f), ToRadians(-90.0f) };
	Vector3 mDemo1SpawnAngularVelocityMax = { ToRadians(90.0f), ToRadians(90.0f), ToRadians(90.0f) };


// Demo 2 variables

	Vector3 mDemo2SpawnPositionMin = { -25.0f, 0.0f, -25.0f };
	Vector3 mDemo2SpawnPositionMax = { 25.0f, 50.0f, 25.0f };

	Vector3 mDemo2SpawnRotationMin = { ToRadians(-180.0f), ToRadians(-180.0f), ToRadians(-180.0f) };
	Vector3 mDemo2SpawnRotationMax = { ToRadians(180.0f), ToRadians(180.0f), ToRadians(180.0f) };

	float mDemo2SpawnScaleMin = 0.5f;
	float mDemo2SpawnScaleMax = 1.0f;
};

#endif // _ENGINE_H_INCLUDED_
