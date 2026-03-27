//========================================================================================================
// Vertex Shader - Transform position into clip space; position and normal into world space; pass on uv
//========================================================================================================

#include "Common.hlsli"

//===============================
// Vertex Shader Input / Output
//===============================

// Input to shader - each vertex has this data
struct Input
{
    float3 position : position; // XYZ position of vertex in model space
    float3 normal : normal;     // XYZ normal at vertex in model space
    float2 uv : uv;             // Texture coordinate at this vertex
};

// Output from shader - passed on to pixel shader
struct Output
{
    float4 clipPosition : SV_Position;      // 2D position of vertex in clip space - this key field is identified by the special semantic "SV_Position" (SV = System Value)
    float3 worldPosition : worldPosition;   // 3D position of vertex in world space - used for lighting   
    float3 worldNormal : worldNormal;       // The surface normal (in world spac) for the vertex pixel - used for lighting
    float2 uv : uv;                         // Texture coordinate for this vertex, used for sample textures
};

//==============
// Shader Code
//==============

// VVeetex shader gets vertices from the mesh one at a time, processes each one and passes resultant data down the pipeline
Output main(Input modelVertex)
{
    Output output; // Output data expected from this shader
    
    // Input vertex position and normal are x, y, z only - need a 4th element to multiply by a 4x4 matrix. Use 1 for point, 0 for a vector
    float4 modelPosition = float4(modelVertex.position, 1);
    float4 modelNormal = float4(modelVertex.normal, 0);
    
    // Multiply position and normal by the world matrix to transform vertex into world space - pass this to pixel shader
    float4 worldPosition = mul(gWorldMatrix, modelPosition);
    output.worldPosition = worldPosition.xyz;
    output.worldNormal = mul(gWorldMatrix, modelNormal).xyz;
    
    // Use combiend view-projection matrix (camera matrices)
    output.clipPosition = mul(gViewProjectionMatrix, worldPosition);
    
    // Pass texture coordinates (UVs) straight to pixel shader
    output.uv = modelVertex.uv;
    
	return output; // Output data sent down the pipeline
}