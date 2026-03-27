//========================================
// Scene Geometry and Layout Preparation
// Scene Rendering and Update
//========================================

#include "Scene.h"

#include "EntityManager.h"
#include "Entity.h"
#include "Camera.h"
#include "Light.h"
#include "RigidBody.h"
#include "SceneGlobals.h"
#include "EngineGUI.h"
#include "EntityTypes.h"

#include "Mesh.h"
#include "RenderMethod.h"
#include "Texture.h"
#include "CBuffer.h"
#include "CBufferTypes.h"
#include "State.h"
#include "RenderGlobals.h"

#include "CollisionDetection.h"

#include "Matrix4x4.h"
#include "Vector3.h"

#include "ColourTypes.h"
#include "Input.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include <sstream>
#include <cmath>
#include <stdexcept>

//============================
// Initialsie Scene Geometry
//============================

// Constructs a demo scene ready to be rendered / updated
Scene::Scene()
{
	// Create the global constant buffers used by this app
	if (!CreateCBuffers()) throw std::runtime_error("Error creating constant buffer");

	// Create entity manager and messenger prior to any entities
	gEntityManager = std::make_unique<EntityManager>();
	gMessenger = std::make_unique<Messenger>();

	// Initialise SpriteFont helper library for text drawing
	//mSpriteBatch = std::make_unique<DirectX::DX11::SpriteBatch>(DX->Context());
	//mSmallFont = std::make_unique<DirectX::DX11::SpriteFont>(DX->Device(), L"tahoma12.spritefont");
	//mMediumFont = std::make_unique<DirectX::DX11::SpriteFont>(DX->Device(), L"tahoma16.spritefont");

	//==========
	// Camera
	//==========

	mCamera = std::make_unique<Camera>();
	mCamera->Transform().Position() = { 0, 100, -200 };
	mCamera->Transform().SetRotation({ ToRadians(15), 0, 0 });
	mCamera->AspectRatio() = static_cast<float>(DX->GetBackBufferWidth()) / DX->GetBackBufferHeight();
	mCamera->FOV() = ToRadians(75);
	mCamera->NearClip() = 1;
	mCamera->FarClip() = 10000;


	//========================
	// Load Entity Templates
	//========================
	
	// Create Entity Templates for scenery
	// Put entity template type in <>
	// Ignoring returned template pointers
	// Parameters are: template name, mesh filename, optional mesh import flags
	gEntityManager->CreateEntityTemplate<EntityTemplate>("Sky", "Sky.fbx", ImportFlags::NoLighting); // Plain texture - no lighting effects
	gEntityManager->CreateEntityTemplate<EntityTemplate>("Floor", "Floor.x");
	gEntityManager->CreateEntityTemplate<EntityTemplate>("Light", "Light.x");
	gEntityManager->CreateEntityTemplate<EntityTemplate>("Cube", "Cube.fbx ");
	gEntityManager->CreateEntityTemplate<EntityTemplate>("Sphere", "Sphere.fbx");
	gEntityManager->CreateEntityTemplate<EntityTemplate>("Wall", "Wall.fbx");

	// Other Entity Templates (Physics Objects)

	// Test for any errors loading entity templates
	if (gEntityManager->GetLastError() != "")
		throw std::runtime_error(gEntityManager->GetLastError());

	gEntityManager->CreateEntity<Entity>("Floor", Matrix4x4({ 0,0,0 }, { 0, 0, 0 }, 5), "Floor"); // No matrix means at origin, no rotation or scaling, 
	gEntityManager->CreateEntity<Entity>("Sky", Matrix4x4({ 0, 0, 0 }, { 0, 0, 0 }, 50), "Sky");  // Position (X,Y,Z), rotation (X,Y,Z) in radians [optional], scale [optional] 

	gEntityManager->CreateEntity<Light>("Light", Matrix4x4({ -500, 500, -1000 }, { 0, 0, 0 }, 40), ColourRGBA(1.0f, 1.0f, 1.0f, 1.0f), "Light");

	//EntityID cubeID = gEntityManager->CreateEntity<Entity>("Cube", Matrix4x4({ 0,10,0 }, { 0,0,0 }, 1), "Static Cube");

	if (gEntityManager->GetLastError() != "")  
		throw std::runtime_error(gEntityManager->GetLastError());

	gAmbientColour = { 1.0f, 1.0f, 1.0f };

	std::tie(std::ignore, mEnvironmentMap) = DX->Textures()->LoadTexture("Media/sea-cube.dds", true);
	RenderState::SetEnvironmentMap(mEnvironmentMap);

	//========
	// GUI
	//========
	mEngineGUI = std::make_unique<EngineGUI>();
}

Scene::~Scene() {};

//=================
// Scene Rendering
//=================

// Render the entire scene
void Scene::Render(float frameTime)
{

	// Setup the rendering viewport to the size of the main window
	D3D11_VIEWPORT vp;
	vp.Width = static_cast<FLOAT>(DX->GetBackBufferWidth());
	vp.Height = static_cast<FLOAT>(DX->GetBackBufferHeight());
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	DX->Context()->RSSetViewports(1, &vp);

	// Put data that's constant for entire frame (e.g. light information) into constant buffer
	
	int i = 0;
	auto lights = gEntityManager->GetTemplate("Light")->Entites();

	for (auto lightID : lights)
	{
		Light* light = gEntityManager->GetEntity<Light>(lightID);
		float lightIntensity = light->mIntensity;
		gPerFrameConstants.lights[i].colour = light->mColour;
		gPerFrameConstants.lights[i].colour = (Vector3)gPerFrameConstants.lights[i].colour * lightIntensity;
		gPerFrameConstants.lights[i].position = light->Transform().Position();
		i++;
	}
	gPerFrameConstants.numLights = i;

	gPerFrameConstants.ambientColour = gAmbientColour;

	gPerFrameConstants.viewportWidth = static_cast<float>(DX->GetBackBufferWidth());
	gPerFrameConstants.viewportHeight = static_cast<float>(DX->GetBackBufferHeight());

	// Send to GPU
	DX->CBuffers()->UpdateCBuffer(gPerFrameConstantBuffer, gPerFrameConstants);

	// Render the scene from the main camera
	
	/*
	mSpriteBatch->Begin(); // Using DirectX helper library SpriteBatch to draw text

	std::stringstream outText;
	outText << "TESTING RENDER";
	mSmallFont->DrawString(mSpriteBatch.get(), outText.str().c_str(), DirectX::XMFLOAT2{10,10}, DirectX::FXMVECTOR{1, 1, 1, 1});
	outText.str("");

	mSpriteBatch->End();
	RenderState::Reset(); // Must call this after using SpriteBatch functions
	*/

	// Rendering is complete, "present" image to screen
	RenderFromCamera(mCamera.get());

	ShowGUIs(frameTime);

	DX->PresentFrame(mLockFPS);
}
	
// Render one viewpoint of the scene from the given camera, helper function for Scene::Render
void Scene::RenderFromCamera(Camera* camera)
{
	// Set camera matrices in the constant buffer and send over to GPU
	gPerCameraConstants.cameraMatrix = camera->Transform();
	gPerCameraConstants.viewMatrix = camera->GetViewMatrix();
	gPerCameraConstants.projectionMatrix = camera->GetProjectionMatrix();
	gPerCameraConstants.viewProjectionMatrix = camera->GetViewProjectionMatrix();
	gPerCameraConstants.cameraPosition = camera->Transform().Position();
	DX->CBuffers()->UpdateCBuffer(gPerCameraConstantBuffer, gPerCameraConstants);

	// Target the back buffer for renderign, clear depth buffer
	DX->Context()->OMSetRenderTargets(1, &DX->BackBuffer(), DX->DepthBuffer());
	DX->Context()->ClearDepthStencilView(DX->DepthBuffer(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Render solid models (render group 0)
	DX->States()->SetRasterizerState(RasterizerState::CullBack); // Default GPU states for non-blended rendering
	DX->States()->SetDepthState(DepthState::DepthOn);
	DX->States()->SetBlendState(BlendState::BlendNone);
	gEntityManager->RenderGroup(0);

	// Render additive blended models (render group 1)
	DX->States()->SetRasterizerState(RasterizerState::CullNone);
	DX->States()->SetDepthState(DepthState::DepthReadOnly);
	DX->States()->SetBlendState(BlendState::BlendAdditive);
	gEntityManager->RenderGroup(1);
}

//===============
// Scene Update
//===============

// Update entire scene. frameTime is the time passed since the last frame
void Scene::Update(float frameTime)
{
	// Update all entities
	gEntityManager->UpdateAll(frameTime);

	float rotation_speed = 1.5f;
	float movement_speed = 75.0f;
	mCamera.get()->Control(frameTime, Key_Up, Key_Down, Key_Left, Key_Right, Key_W, Key_S, Key_A, Key_D, movement_speed, rotation_speed);

	// Detecting collisions substep amount of times each frame - reduce chance of intersections when updating object movement next frame
	
	for (int subSteps = 0; subSteps < 1; subSteps++)
		gCollisionDetection->CheckAllCollisions();

	if (gCurrentDemo == 1)
	{
		Demo1(frameTime);
	}

	if (gCurrentDemo == 2)
	{
		Demo2(frameTime, mCamera.get());
	}
}

void Scene::ShowGUIs(float frameTime)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//ImGui::ShowDemoWindow();

	mEngineGUI->ShowWindow();

	//===========================
	// Finalise ImGUI for frame
	//===========================

	if (gCurrentDemo == 1)
	{
		mEngineGUI->ShowDemoWindow1(frameTime);
	}
	if (gCurrentDemo == 2)
	{
		mEngineGUI->ShowDemoWindow2(frameTime);
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

//--------------------------------------------------------------------------------------
// Helper functions
//--------------------------------------------------------------------------------------

// Draw given text at the given 3D point, also pass camera in use. Optionally centre align and colour the text
void Scene::DrawTextAtWorldPoint(const Vector3& point, std::string text, Camera* camera, bool centreAlign, ColourRGB colour)
{
	auto pixelPt = camera->PixelFromWorldPoint(point, DX->GetBackBufferWidth(), DX->GetBackBufferHeight());
	if (pixelPt.z >= camera->NearClip())
	{
		if (centreAlign)
		{
			auto textSize = mSmallFont->MeasureString(text.c_str());
			pixelPt.x -= DirectX::XMVectorGetX(textSize) / 2;
		}
		mSmallFont->DrawString(mSpriteBatch.get(), text.c_str(), DirectX::XMFLOAT2{ pixelPt.x, pixelPt.y }, DirectX::FXMVECTOR{ colour.r, colour.g, colour.b, 0 });

	}
}

void Demo1(float frameTime)
{

}

void Demo2(float frameTime, Camera* camera)
{
	if (KeyHit(Mouse_LButton))
	{
		Vector3 worldPoint = camera->WorldPointFromPixel(GetMouse(), DX->GetBackBufferWidth(), DX->GetBackBufferHeight());
		Vector3 direction = Normalise(worldPoint - camera->Transform().Position());
		float launchSpeed = 100;

		Vector3 launchVector = camera->Transform().ZAxis() * launchSpeed;
		Vector3 spawnLocation = worldPoint + direction * 10;
		gEntityManager->CreateEntity<RigidBody>("Sphere", Matrix4x4(spawnLocation, { 0, 0, 0 }, 0.90f), "Sphere", launchVector, Vector3{ 0,0,0 });  // Position (X,Y,Z), rotation (X,Y,Z) in radians [optional], scale [optional] 

	}
}

void ResetScene()
{
	auto& entities = gEntityManager->GetEntities();
	for (auto it = entities.begin(); it != entities.end();)
	{
		auto entity = it->second.get();
		it++;

		if (entity->GetName() != "Floor" && entity->GetName() != "Sky" && entity->GetName() != "Light")
			gEntityManager->DestroyEntity(entity->GetID());
	}
}