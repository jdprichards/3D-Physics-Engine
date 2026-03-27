#include "EngineGUI.h"
#include "Entity.h"
#include "RigidBody.h"

#include <ctype.h>          // toupper
#include <limits.h>         // INT_MIN, INT_MAX
#include <math.h>           // sqrtf, powf, cosf, sinf, floorf, ceilf
#include <stdio.h>          // vsnprintf, sscanf, printf
#include <stdlib.h>         // NULL, malloc, free, atoi
#if defined(_MSC_VER) && _MSC_VER <= 1500 // MSVC 2008 or earlier
#include <stddef.h>         // intptr_t
#else
#include <stdint.h>         // intptr_t
#endif

void EngineGUI::ShowWindow()
{
	// Demonstrate the various window flags. Typically you would just use the default!
	static bool no_titlebar = false;
	static bool no_scrollbar = false;
	static bool no_menu = true;
	static bool no_move = false;
	static bool no_resize = false;
	static bool no_collapse = false;
	static bool no_close = false;
	static bool no_nav = false;
	static bool no_background = false;
	static bool no_bring_to_front = false;

	ImGuiWindowFlags window_flags = 0;
	if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
	if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
	if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
	if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
	if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
	if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
	if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
	if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
	if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

	ImGui::Begin("Game Engine", 0, window_flags);

	//ImGui::PushItemWidth(ImGui::GetFontSize() * -12);           // Use fixed width for labels (by passing a negative value), the rest goes to widgets. We choose a width proportional to our font size.

	if (ImGui::Button("Reset"))
	{
		gCurrentDemo = 0;

		ResetScene();
	}
	ImGui::SameLine();
	if (ImGui::Button("Demo 1"))
	{
		gCurrentDemo = 1;

		ResetScene();

		gEntityManager->CreateEntity<Entity>("Wall", Matrix4x4({ 0, 10, 50 }, { 0, 0, 0 }, 2.5f), "Wall");
		gEntityManager->CreateEntity<Entity>("Wall", Matrix4x4({ 0, 10, -50 }, { 0, 0, 0}, 2.5f), "Wall");
		gEntityManager->CreateEntity<Entity>("Wall", Matrix4x4({ 50, 10, 0 }, { 0, ToRadians(90), 0}, 2.5f), "Wall");
		gEntityManager->CreateEntity<Entity>("Wall", Matrix4x4({ -50, 10, 0 }, { 0, ToRadians(90), 0}, 2.5f), "Wall");
	}
	ImGui::SameLine();
	if (ImGui::Button("Demo 2"))
	{
		gCurrentDemo = 2;

		ResetScene();

		for (int i = 0; i < 25; i++)
		{
			Vector3 randomPosition = { Random(mDemo2SpawnPositionMin.x, mDemo2SpawnPositionMax.x), Random(mDemo2SpawnPositionMin.y, mDemo2SpawnPositionMax.y) , Random(mDemo2SpawnPositionMin.z, mDemo2SpawnPositionMax.z) };
			Vector3 randomRotation = { Random(mDemo2SpawnRotationMin.x, mDemo2SpawnRotationMax.x), Random(mDemo2SpawnRotationMin.y, mDemo2SpawnRotationMax.y) , Random(mDemo2SpawnRotationMin.z, mDemo2SpawnRotationMax.z) };
			float randomScale = Random(mDemo2SpawnScaleMin, mDemo2SpawnScaleMax);
			randomRotation = { 0,0,0 };
			gEntityManager->CreateEntity<RigidBody>("Cube", Matrix4x4{ randomPosition, randomRotation, randomScale }, "Cube");
		}

	}

	ImGui::Separator();

	ImGui::ColorEdit3("Ambient Light", &gAmbientColour.r, ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::Separator();
	// Show all objects in scene
	static int selectedID = -1;
	static int selected = -1;

	ImGui::Text("Models:");
	int n = 0;
	for (auto& [id, entity] : gEntityManager->GetEntities())
	{
		const char* objectName = entity->GetName().c_str();

		std::string stringID = std::to_string(entity->GetID());
		const char* objectID = stringID.c_str();

		std::string objectString = entity->GetName() + "(" + stringID + ")";

		const char* objectChar = objectString.c_str();


		if (ImGui::Selectable(objectChar, selected == n))
		{
			selectedID = entity->GetID();
			selected = n;
		}

		n++;
	}

	if (ImGui::Button("Spawn Cube"))
	{
		Vector3 randomPosition = Vector3{ Random(-25.0f , 25.0f), Random(5.0f, 25.0f), Random(-25.0f, 25.0f) };
		Vector3 randomRotation = Vector3{ ToRadians(Random(-180.0f, 180.0f)), ToRadians(Random(-180.0f, 180.0f)), ToRadians(Random(-180.0f, 180.0f)) };
		//randomPosition = { 0,20,0 };
		//randomRotation = { 0,0,0 };

		Vector3 randomVelocity = Vector3{ Random(-25.0f , 25.0f), Random(0.0f, 25.0f), Random(-25.0f, 25.0f) };
		Vector3 randomAngularVelocity = Vector3{ ToRadians(Random(-180.0f, 180.0f)), ToRadians(Random(-180.0f, 180.0f)), ToRadians(Random(-180.0f, 180.0f)) };

		gEntityManager->CreateEntity<RigidBody>("Cube", Matrix4x4(randomPosition, randomRotation, 1), "Cube", randomVelocity, randomAngularVelocity);  // Position (X,Y,Z), rotation (X,Y,Z) in radians [optional], scale [optional] 
	}
	ImGui::SameLine();
	if (ImGui::Button("Spawn Sphere"))
	{
		Vector3 randomVelocity = Vector3{ Random(-25.0f , 25.0f), Random(10.0f, 25.0f), Random(-25.0f, 25.0f) };
		Vector3 randomAngularVelocity = Vector3{ ToRadians(Random(-180.0f, 180.0f)), ToRadians(Random(-180.0f, 180.0f)), ToRadians(Random(-180.0f, 180.0f)) };
		gEntityManager->CreateEntity<RigidBody>("Sphere", Matrix4x4({ 0, 20, 0 }, { 0, 0, 0 }, 1), "Sphere", randomVelocity, randomAngularVelocity);  // Position (X,Y,Z), rotation (X,Y,Z) in radians [optional], scale [optional] 
	}

	if (selectedID != -1 && gEntityManager->GetEntity(selectedID) != nullptr)
	{
		Entity* selectedEntity = gEntityManager->GetEntity(selectedID);

		EEntityType objectType = selectedEntity->EntityType();

		ImGui::Separator();

		ImGui::Text("Model Information");

		const char* objectName = selectedEntity->GetName().c_str();

		std::string stringID = std::to_string(selectedEntity->GetID());
		const char* objectID = stringID.c_str();

		ImGui::Text("Selected Model: %s", objectName);
		ImGui::Text("Model ID: %s", objectID);
		
		std::string strObjectType = entityTypeMap[objectType] ;
		const char* charObjectType = strObjectType.c_str();
		ImGui::Text("Object Type: %s", charObjectType);

		
		ImGui::InputFloat3("Position", &selectedEntity->Transform().Position().x);

		Vector3 angle = selectedEntity->Transform().GetRotation();
		Vector3 scale = selectedEntity->Transform().GetScale();

		ImGui::SliderFloat3("Rotation", &angle.x, ToRadians(0), ToRadians(360.0f));
		ImGui::SliderFloat3("Scale", &scale.x, 0.01, 10);

		selectedEntity->Transform().SetRotation(angle);
		selectedEntity->Transform().SetScale(scale);
		
		ImGui::Separator();
		ImGui::Text("Template Information");
		EntityTemplate& objectTemplate = selectedEntity->Template();
		const char* templateType = objectTemplate.GetType().c_str();
		ImGui::Text("Template Type: %s", templateType);
		
		if (objectType == EEntityType::Light)
		{
			Light* lightObject = dynamic_cast<Light*>(selectedEntity);
			ImGui::Separator();
			ImGui::Text("Light Data");
			ImGui::InputFloat("Light Intensity", &lightObject->mIntensity);
			ImGui::InputFloat4("Light Colour", &lightObject->mColour.r);
		}

		// Avoid Lights and the sky showing mass data
		if (objectType != EEntityType::Light && selectedEntity->Template().GetType() != "Sky")
		{
			ImGui::Separator();
			ImGui::InputFloat("Mass", &selectedEntity->Mass());
			ImGui::InputFloat("Density", &selectedEntity->Density());
			ImGui::InputFloat("Volume", &selectedEntity->Volume());

			ImGui::InputFloat3("Center of Mass", &selectedEntity->CenterOfMass().x);
		}

		if (objectType == EEntityType::RigidBody)
		{
			RigidBody* rigidBodyObject = dynamic_cast<RigidBody*>(selectedEntity);
			Vector3 scale = rigidBodyObject->Transform().GetScale();
			ImGui::Separator();
			ImGui::Text("Rigid Body Data");
			
			ImGui::InputFloat3("Velocity", &rigidBodyObject->Velocity().x);
			ImGui::InputFloat3("Angular Velocity", &rigidBodyObject->AngularVelocity().x);

			ImGui::InputFloat3("Acceleration", &rigidBodyObject->Accelleration().x);
			ImGui::InputFloat3("Angular Acceleration", &rigidBodyObject->AngularAcceleration().x);

			ImGui::InputFloat3("Resultant Force", &rigidBodyObject->ResultantForce().x);
			ImGui::InputFloat3("Resultant Torque", &rigidBodyObject->ResultantTorque().x);

			ImGui::Text("Inertia Tensor");
			ImGui::InputFloat3("", &rigidBodyObject->InertiaTensor().e00);
			ImGui::InputFloat3("", &rigidBodyObject->InertiaTensor().e10);
			ImGui::InputFloat3("", &rigidBodyObject->InertiaTensor().e20);

			ImGui::Separator();
		}

		if (objectType != EEntityType::Light && selectedEntity->Template().GetType() != "Sky")
		{
			ImGui::Text("Bounding Volume Data");

			if (selectedEntity->CollisionType() == ECollisionType::AABB)
			{
				AABB* aabb = selectedEntity->GetBoundingVolume<AABB>();
				ImGui::Text("AABB Bounding Box");
				ImGui::InputFloat3("Max Point", &aabb->MinAABB().x);
				ImGui::InputFloat3("Min Point", &aabb->MinAABB().x);
			}

			if (selectedEntity->CollisionType() == ECollisionType::OBB)
			{
				OBB* obb = selectedEntity->GetBoundingVolume<OBB>();

			}

			if (selectedEntity->CollisionType() == ECollisionType::BoundingSphere)
			{
				BoundingSphere* boundingSphere = selectedEntity->GetBoundingVolume<BoundingSphere>();
				ImGui::Text("Spherical Bounding Box");
				ImGui::InputFloat("Sphere Bounding Box Radius", &boundingSphere->Radius());
				ImGui::InputFloat3("Sphere Bounding Box Center", &boundingSphere->Center().x);
			}

			if (selectedEntity->CollisionType() == ECollisionType::Plane)
			{
				Plane* plane = selectedEntity->GetBoundingVolume<Plane>();
				ImGui::Text("Collision Plane");
				ImGui::InputFloat3("Plane Normal", &plane->Normal().x);
			}
		}
	}

	ImGui::End();

}


void EngineGUI::ShowDemoWindow1(float frameTime)
{
	// Demonstrate the various window flags. Typically you would just use the default!
	static bool no_titlebar = false;
	static bool no_scrollbar = false;
	static bool no_menu = true;
	static bool no_move = false;
	static bool no_resize = false;
	static bool no_collapse = false;
	static bool no_close = false;
	static bool no_nav = false;
	static bool no_background = true;
	static bool no_bring_to_front = false;

	ImGuiWindowFlags window_flags = 0;
	if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
	if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
	if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
	if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
	if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
	if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
	if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
	if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
	if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

	ImGui::Begin("Demo 1");
	ImGui::InputFloat("Spawn Rate (s)", &mDemo1SpawnRate);
	ImGui::InputFloat("Entity Lifetime", &mDemo1LifeTime);
	ImGui::Separator();
	ImGui::InputFloat3("Min Spawn Position", &mDemo1SpawnPositionMin.x);
	ImGui::InputFloat3("Max Spawn Position", &mDemo1SpawnPositionMax.x);
	ImGui::InputFloat3("Min Spawn Rotation", &mDemo1SpawnRotationMin.x);
	ImGui::InputFloat3("Max Spawn Rotation", &mDemo1SpawnRotationMax.x);
	ImGui::InputFloat("Min Spawn Scale", &mDemo1SpawnScaleMin);
	ImGui::InputFloat("Max Spawn Scale", &mDemo1SpawnScaleMax);
	ImGui::Separator();
	ImGui::InputFloat3("Min Spawn Velocity (M/s)", &mDemo1SpawnVelocityMin.x);
	ImGui::InputFloat3("Max Spawn Velocity (M/s)", &mDemo1SpawnVelocityMax.x);
	ImGui::InputFloat3("Min Spawn Angular Velocity (rad/s)", &mDemo1SpawnAngularVelocityMin.x);
	ImGui::InputFloat3("Min Spawn Angular Velocity (rad/s)", &mDemo1SpawnAngularVelocityMax.x);
	ImGui::End();

	mDemo1SpawnTime += frameTime;
	if (mDemo1SpawnTime > mDemo1SpawnRate)
	{
		Vector3 randomPosition = {Random(mDemo1SpawnPositionMin.x, mDemo1SpawnPositionMax.x), Random(mDemo1SpawnPositionMin.y, mDemo1SpawnPositionMax.y) , Random(mDemo1SpawnPositionMin.z, mDemo1SpawnPositionMax.z) };
		Vector3 randomRotation = { Random(mDemo1SpawnRotationMin.x, mDemo1SpawnRotationMax.x), Random(mDemo1SpawnRotationMin.y, mDemo1SpawnRotationMax.y) , Random(mDemo1SpawnRotationMin.z, mDemo1SpawnRotationMax.z) };
		float randomScale = Random(mDemo1SpawnScaleMin, mDemo1SpawnScaleMax);

		Vector3 randomVelocity = { Random(mDemo1SpawnVelocityMin.x, mDemo1SpawnVelocityMax.x), Random(mDemo1SpawnVelocityMin.y, mDemo1SpawnVelocityMax.y) , Random(mDemo1SpawnVelocityMin.z, mDemo1SpawnVelocityMax.z) };
		Vector3 randomAngularVelocity = {Random(mDemo1SpawnAngularVelocityMin.x, mDemo1SpawnAngularVelocityMax.x), Random(mDemo1SpawnAngularVelocityMin.y, mDemo1SpawnAngularVelocityMax.y) , Random(mDemo1SpawnAngularVelocityMin.z, mDemo1SpawnAngularVelocityMax.z) };

		EntityID sphereID = gEntityManager->CreateEntity<RigidBody>("Sphere", Matrix4x4(randomPosition, randomRotation, randomScale), "Sphere", randomVelocity, randomAngularVelocity);  // Position (X,Y,Z), rotation (X,Y,Z) in radians [optional], scale [optional] 
		RigidBody* sphereEntity = gEntityManager->GetEntity<RigidBody>(sphereID);
		sphereEntity->SetLifeTime(mDemo1LifeTime);
		
		mDemo1SpawnTime = 0.0f;
	}
}

void EngineGUI::ShowDemoWindow2(float frameTime)
{
	// Demonstrate the various window flags. Typically you would just use the default!
	static bool no_titlebar = false;
	static bool no_scrollbar = false;
	static bool no_menu = true;
	static bool no_move = false;
	static bool no_resize = false;
	static bool no_collapse = false;
	static bool no_close = false;
	static bool no_nav = false;
	static bool no_background = true;
	static bool no_bring_to_front = false;

	ImGuiWindowFlags window_flags = 0;
	if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
	if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
	if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
	if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
	if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
	if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
	if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
	if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
	if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

	ImGui::Begin("Demo 2");

	ImGui::End();

}
