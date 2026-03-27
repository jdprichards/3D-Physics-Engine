//========================================
// Scene Geometry and Layout preparation
//========================================

#ifndef _SCENE_H_INCLUDED_
#define _SCENE_H_INCLUDED_

#include "ColourTypes.h"
#include "EngineGUI.h"

#define NOMINMAX // Use NOMINMAX to stop Microsoft headers defining "min" and "max" which breaks some libraries

#include <d3d11.h>
#include <SpriteFont.h>

#include <string>
#include <memory>

// Forward declarations of various lasses allows us to use pointers to those classes before those classes have been fully declared
// Will help reduce the number of include files, which minimises dependenices and speeds up compilation
//
// Note if class is containing unique_ptr to a forward decleration then this class must also have a destructor
// signature here and a declaration in the cpp file - is required even if destructor is default / empty

class EntityManager;
class EngineGUI;
class Entity;
class Camera;
class Mesh;


//==============
// Scene Class
//==============

class Scene
{
public:
	//=============================
	// Construction / Destruction
	//=============================

	// Constructs a demo scene ready to be rendered / updated
	Scene();

	// No destruction required but see common on forward declaration
	~Scene();

	//===========
	// Usage
	//===========

	// Render Entire Scene
	void Render(float frameTime);

	// Update Entire Scene. fraemTime is time passed since last frame
	void Update(float frameTime);

	void ShowGUIs(float frameTime);


private:
	//============================
	// Private Helper Functions
	//============================

	// Render one viewpoint of the scene from the given camera, helper function for Scene::Render
	void RenderFromCamera(Camera* camera);

	// Draw given text at the given 3D point, also pass camera in use. Optionally centre allign and colour the text
	void DrawTextAtWorldPoint(const Vector3& point, std::string text, Camera* camera, bool centreAlign = false, ColourRGB  colour  = { 1, 1, 1 });

private:
	//================
	// Private Data
	//================

	std::unique_ptr<Camera> mCamera;

	// Additional light information
	ID3D11ShaderResourceView* mEnvironmentMap = {};

	bool mFreeCamera = false;

	// Lock FPS to monitor refresh rate, which will set it so 60 / 120 / 144 / 240 fps
	bool mLockFPS = true;

	// DirectXTK SpriteFont text drawing variables
	std::unique_ptr<DirectX::DX11::SpriteBatch> mSpriteBatch;
	std::unique_ptr<DirectX::DX11::SpriteFont> mSmallFont;
	std::unique_ptr<DirectX::DX11::SpriteFont> mMediumFont;

	std::unique_ptr<EngineGUI> mEngineGUI;
};

void Demo1(float frameTime);
void Demo2(float frameTime, Camera* camera);
void ResetScene();
#endif // !_SCENE_H_INCLUDED_
