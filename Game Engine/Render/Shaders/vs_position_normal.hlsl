//==========================================================================================
// Vertex Shader - Transform position into clipspace; position and normal into world space
//==========================================================================================

#include "Common.hlsli"

//===============================
// Vertex Shader Input / Output
//===============================

// Input to shader - each vertex has this data
struct Input
{
    float3 position : position;     // XYZ position of vertex in model space
    float3 normal : normal;         // XYZ normal at vertex in model space
};

// Output from shader - passed on to pixel shader
struct Output
{
    float4 clipPosition : SV_Position;      // 2D position of vertex in clip space - this key field is identified by special semantic "SV_Position" (SV = System Value)
    float3 worldPosition : worldPosition;   // 3D position of vertex in world space - used for lighting
    float3 worldNormal : worldNormal;       // The surface normal (in world space) for vertex pixel - used for lighting
};

//===============
// Shader Code
//===============

// Vertex shader gets vertices from mesh one at a time, processes each one and passes resultant data to the pixel shader
Output main(Input modelVertex)
{
    Output output; // Output data expected from this shader
    
    // Input vertex position and normal are x, y, z only - need 4th element to multiply by a 4x4 matrix. Use 1 for point, 0 for vector
    float4 modelPosition = float4(modelVertex.position, 1);
    float4 modelNormal = float4(modelVertex.normal, 0);
    
    // Multiply position and normal by the world matrix to transform vertex into world space - pass this data to the pixel shader for lighting calculations
    float4 worldPosition = mul(gWorldMatrix, modelPosition);
    output.worldPosition = worldPosition.xyz;
    output.worldNormal = mul(gWorldMatrix, modelNormal).xyz;
    
    // Use combined view-projection matrix (camera matrices) to transform vertex into 2D clip space -  pass on to pixel shader
    output.clipPosition = mul(gViewProjectionMatrix, worldPosition);
    
    return output; // Output data sent down the pipeline
}