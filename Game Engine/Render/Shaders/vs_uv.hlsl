//==================================================================
// Vertex Shader - Transform position into clip space; pass on uv
//==================================================================

#include "Common.hlsli"

//================================
// Vertex Shader Input / Output
//================================

// Input to shader - each vertex has this data
struct Input
{
    float3 position : position; // XYZ position of vertex in model space
    float2 uv : uv;             // Texture coordinates at this vertex
};

// Output from shader - passed on to pixel shader
struct Output
{
    float4 clipPosition : SV_Position;  // 2D position of vertex in clip space - this key field is identified by semantic "SV_Position" ( SV = System Value)
    float2 uv : uv;                     // Texure coordinate for this vertex, used to sample textures
};

//==============
// Shader Code
//==============

// Vertex shader gets vertices from mesh one at a time, processes each one and passes resulant data on to the pixel shade
Output main(Input modelVertex)
{
    Output output; // Output data expected from this shader
    
    // Input vertex position is x, y, z only - need a 4th element  to multiply by a 4x4 matrix. Use 1 for point, 0 for vector
    float4 modelPosition = float4(modelVertex.position, 1);
    
    // Multiply position by the the world matrix to transform vertex into world space
    float4 worldPosition = mul(gWorldMatrix, modelPosition);
    
    // Use combined view-projection matrix (camera matrices) to transform into 2D clip space, pass on to pixel shader
    output.clipPosition = mul(gViewProjectionMatrix, worldPosition);
    
    // Pass texture coordinates (UVs) on to pixel shader
    output.uv = modelVertex.uv;

    return output; // Output data sent down the pipeline

}