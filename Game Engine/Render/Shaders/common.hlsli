//======================================================================
// Common include file for all shaders
//======================================================================
// Using include files to define constant buffers avaliable to shaders

// Prevent using this include file being included multiple times
#ifndef _COMMON_HLSLI_DEFINED_
#define _COMMON_HLSLI_DEFINED_

//=============
// Constants
//=============

static const float PI = 3.14159265f;
static const int MAX_LIGHTS = 40;

struct SLight
{
    float3 gPosition;
    float padding1;
    
    float3 gColour;
    float padding2;
};

//===================
// Constant Buffers
//===================
// These structures are called "constant buffers" - a way of passing variables over from C++ to the GPU
// They are called constants, meaning they're constant for the duration of a single GPU draw call
// These "constants" correspond to variables in C++ that will change per-mesh or per-frame, etc.
//
// Constant buffers are not structs, don't use the name of the constant buffer in shaders
// Essential a collective of global varibabes (hence the 'g')

// The constants that need to be sent ot the GPU each frame (PerFrameConstants in C++)
// These variables must match exactly the PerFrameConstants structure in C++
// As well as be in groups of 16 bytes (for example float4 or float3 + float or Matrix4x4) so padding may be needed
cbuffer PerFrameConstants : register(b0) // b0 gives the constant buffer the slot 0 - used in C++
{
    SLight lights[MAX_LIGHTS];
    
    float gViewportWidth; // Using viewport width and height as padding - data needs to fit into blocks of 4 floats / ints (16 bytes)
    float gViewportHeight;
    float2 padding3;
    
    float3 gAmbientColour;
    float numLights;
}

// The constants that need to be sent to the GPU for each camera in each frame
// These variables must match exactly the PerCameraConstants structure in the C++ code
cbuffer PerCameraConstants : register(b1) // The b1 gives the constant buffer the slot 1
{
    float4x4 gCameraMatrix;
    float4x4 gViewMatrix;
    float4x4 gProjectionMatrix;
    float4x4 gViewProjectionMatrix;
    
    float3 gCameraPosition;
    float padding4;
}

static const int MAX_BONES = 64;

// The constants that need to change for each mesh rendered. e.g. transformation matrices (PerMeshConstants in C++)
// These variables must match exactly the PerMeshConstants structure in C++
cbuffer PerMeshConstants : register(b2)
{
    float4x4 gWorldMatrix;
    float4 gMeshColour; // Per-mesh colour, used to tint it
    float4x4 gBoneMatrices[MAX_BONES];
}

// The constants that need to change for each submesh to suit the material it uses (PerMaterialConstants in C++)
// These variables must match exact the PerMaterialConstants in C++
cbuffer PerMaterialConstants : register(b3)
{
    float4 gMaterialDiffuseColour;
    
    float3 gMaterialSpecularColour;
    float gMaterialSpecularPower;
    
    float gParallaxDepth;
    float3 padding5;
}

#endif // _COMMON_HLSLI_DEFINED_