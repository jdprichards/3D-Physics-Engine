//=================================
// Constant Buffer Related Types
//=================================
// Constant buffers allow us to pass data from C++ to shaders
//
// Different constant buffers are used fir different frequency of update. E.g. the per-frame constant buffer holds data that
// only changes with each frame such as light positions, where as the per-material constant buffer changes for each new material
// rendered within a single frame - it might hold things such as material diffuse colour.

#ifndef _C_BUFFER_TYPES_H_INCLUDED_
#define _C_BUFFER_TYPES_H_INCLUDED_

#include "MeshTypes.h"
#include "Matrix4x4.h"
#include "Vector3.h"
#include "Vector2.h"
#include "ColourTypes.h"

#define NOMINMAX // Use NOMINMAX to stop certain Microsoft headers defining "min" and "max" which breaks some libraries

#include <d3d11.h>

static const int MAX_LIGHTS = 40;

//==================
// Light Structure
//==================
struct SLight
{
	Vector3 position;
	float padding1 = {};

	ColourRGB colour;
	float padding2 = {};
};

//===============
// Structures
//===============

// Per-frame buffer holds data that is constant for an entire frame, updated from C++ to the GPU shader once per frame
// We hold te data together in a structure and send the whole thing to a "constant buffer" on the GPU each from when
// we have finished updating the scene. There is a structure in the shader code that exactly matches this one
struct PerFrameConstants
{
	SLight lights[MAX_LIGHTS];

	float viewportWidth = 1280; // Using viewport width and height as padding - data neeeds to fit into blocks of 4 floats / ints (16 byte blocks)
	float viewportHeight = 720;
	Vector2 padding3 = {};

	ColourRGB ambientColour = { 0, 0, 0 };
	int numLights = 0;
};


// Per-camera buffer holds data that is constant whilst rendering the scene from a single camera. We might render the
// scene more than once in a frame, e.g. for portals or mirrors. Other than that it works in the same way as the per-frame buffer above
struct PerCameraConstants
{
	// These are the matrices used to position the camera
	Matrix4x4 cameraMatrix = Matrix4x4::Identity; // World matrix for camera (inverse of view matrix below)
	Matrix4x4 viewMatrix = Matrix4x4::Identity;
	Matrix4x4 projectionMatrix = Matrix4x4::Identity;
	Matrix4x4 viewProjectionMatrix = Matrix4x4::Identity; // The above two matrices muultiplied together to combine their effects

	Vector3 cameraPosition = { 0, 0, 0 };
	float padding4 = {};
};

// Data required to render the next mesh in the scene, most importantly its transformations matrices. This data is updated and sent to
// the GPU several times every frame, once per mesh (or mesh type). Other than that it works in the same way as the per-frame buffer
struct PerMeshConstants
{
	Matrix4x4 worldMatrix = Matrix4x4::Identity;
	ColourRGBA meshColour = { 1, 1, 1, 1 }; // Per-mesh colour, typically used to tint it (e.g. tinting light meshes to the colour of the light they emit)
	Matrix4x4 boneMatrices[MAX_BONES] = {};
};


// Data required to render the material used by the next submesh in the scene This data is updated and sent to the GPU several times
// each frame, once per submesh. Other than that it works in the same way as the above buffers
struct PerMaterialConstants
{
	ColourRGBA diffuseColour = { 1, 1, 1, 1 }; // Alpha will be imported from mesh file
	
	ColourRGB specularColour = { 0, 0, 0 };
	float specularPower = 0;
	
	float parallaxDepth = 0;
	Vector3 padding5 = {};
};

#endif // !_C_BUFFER_TYPES_H_INCLUDED_
