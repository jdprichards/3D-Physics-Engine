//==========================
// Encapsulating a Camera
//==========================
// Holds the transformation matrix for camera position / rotation, near / far clip and field of view
// Can use these to create view and projection matrices as required for rendering
// Transformation can be manpulated in a similar way to any modes
// Also supports functions for 3D->2D  "picking" (world coordinates <-> pixel position)

#ifndef _CAMERA_H_INCLUDED_
#define _CAMERA_H_INCLUDED_


#include "Vector2.h"
#include "Vector3.h"
#include "Matrix4x4.h"
#include "Input.h"
#include <numbers>

class Camera
{
public:
	
	//=========================
	// Construction and Usage
	//=========================

	// Constructor - Initialise al settings, sensible defaults provided for everything
	Camera(Vector3 position = { 0,0,0 }, Vector3 rotation = { 0,0,0 }, float fov = std::numbers::pi_v<float> / 3,
		float aspectRatio = 4.0f / 3.0f, float nearClip = 0.1f, float farClip = 10000.0f)
		: mFOVx(fov), mAspectRatio(aspectRatio), mNearClip(nearClip), mFarClip(farClip)
	{
		mTransform.Position() = position;
		mTransform.SetRotation(rotation);
		UpdateMatrices();
	}

	void Control(float frameTime,
		KeyCode turnUp, KeyCode turnDown, KeyCode turnLeft, KeyCode turnRight,
		KeyCode moveForward, KeyCode moveBackward, KeyCode moveLeft, KeyCode moveRight,
		float movementSpeed, float rotationSpeed);


	//================
	// Data Access
	//================

	// Direct access to Camera's Transformation Matrix
	Matrix4x4& Transform() { return mTransform; }

	// Direct Access to Camera Settings
	float& AspectRatio() { return mAspectRatio; }
	float& FOV() { return mFOVx; }
	float& NearClip() { return mNearClip; }
	float& FarClip() { return mFarClip; }

	// Camera Matrices used for Rendering. These getters only update member variables when requested
	Matrix4x4 GetViewMatrix() { UpdateMatrices(); return mViewMatrix; }
	Matrix4x4 GetProjectionMatrix() { UpdateMatrices(); return mProjectionMatrix; }
	Matrix4x4 GetViewProjectionMatrix() { UpdateMatrices(); return mViewProjectionMatrix; }


	//==================
	// Camera Picking
	//==================
	// Convert 2D pixel positions to and from 3D world positions

	// Return pixel coordinates corresponding to given world point when viewing from this camera
	// Pass viewport width and height. Returned Vector3 contains pixel coordinates in x, y and z
	// distances to the world point in Z. If Z distance is less than near clip, the world point is
	// behind the camera and the 2D x and y coordinates can be ignored.
	Vector3 PixelFromWorldPoint(Vector3 worldPoint, unsigned int viewportWidth, unsigned int viewportHeight);

	Vector3 WorldPointFromPixel(Vector2i pixelPoint, int viewportWidth, int viewportHeight);

	// Return the size of a pixel at given Z distance. Allows to convert 2D size of areas on screen to actual sizes in the world
	// Pass viewport width and height
	Vector2 PixelSizeInWorldSpace(float Z, unsigned int viewportWidth, unsigned int viewportHeight);

private:
	//===============
	// Private Data
	//===============

	// Update matrices used for the camera in the rendering pipeline
	void UpdateMatrices();

	// Camera settings: field of view, aspect ratio, near and far clip plane distances
	float mFOVx; // FOVx is the viewing angle in (radians = degrees * (PI / 180) from left -> right (high values give fish-eye like look)
	float mAspectRatio; // Aspect ratio is the screen width / height such as (4/3 or 16/ 9)
	float mNearClip; // Near and far clip are the range along the z axis that can be rendered (world units)
	float mFarClip; // __""__

	// Camera Matrices for Positioning and Rendering
	Matrix4x4 mTransform = Matrix4x4::Identity; // Easiest to treat the camera like a model and give it a transformation ("world" matrix)
	Matrix4x4 mViewMatrix; // the view matrix can be used in the shaders is the inversse of the world matrix

	Matrix4x4 mProjectionMatrix; // Projection matrix holds the field of view and the near/far clip distances
	Matrix4x4 mViewProjectionMatrix; // The View and Projection matrix can be multiplied together, saving time on the GPU shader

};

#endif // !_CAMERA_H_INCLUDED_
