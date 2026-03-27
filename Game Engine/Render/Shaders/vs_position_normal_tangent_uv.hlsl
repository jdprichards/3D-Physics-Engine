//===========================================================================================================
// Vertex Shader - Transform position into clip space; position, normal, tangent into world space; pass UV
//===========================================================================================================

#include "Common.hlsli"

//===============================
// Vertex Shader Input / Output
//===============================

// Input to shader - each vertex has this data
struct Input
{
    float3 position : position; // XYZ position of vertex in model space
    float3 normal : normal;     // XYZ normal of vertex in model space
    float3 tangent : tangent;   // XYZ tangent of vertex in model space
    float2 uv : uv;             // Texture coordinate of this vertex
};

// Output from shader - passed down the pipeline
struct Output
{
    float4 clipPosition : SV_Position;      // 2D position of vertex in clip space - this key field is identified by the special semantic "SV_Position" (SV = System Value)
    float3 worldPosition : worldPosition;   // 3D position of vertex in world space - used for lighting
    float3 worldNormal : worldNormal;       // The surface normal (in world space) for this vertex - used for lighting
    float3 worldTangent : worldTangent;     // The surface tangent (in world space) for this vertex - used for normal / parallax mapping
    float2 uv : uv;                         // Texture coordinates for this vertex, used to sample textures
};

//==============
// Shader Code
//==============

// Vertex shader get vertices from the mesh one at a time, processes each one and passes the resultant data on to pixel shader
Output main(Input modelVertex)
{
    Output output; // Output data expected from this shader
    
    // Input vertex posiiton, normal and tagent are x, y, z only - need a 4th element to multiply by a 4x4 matrix. Use 1 for point 0 for vector
    float4 modelPosition = float4(modelVertex.position, 1);
    float4 modelNormal = float4(modelVertex.normal, 0);
    float4 modelTangent = float4(modelVertex.tangent, 0);
    
    // Multiply position, normal and tagent by world matrix to trasform into world space
    float4 worldPosition = mul(gWorldMatrix, modelPosition);
    output.worldPosition = worldPosition.xyz;
    output.worldNormal = mul(gWorldMatrix, modelNormal).xyz;
    output.worldTangent = mul(gWorldMatrix, modelTangent).xyz;
    
    // Use combined view-projection matrix (camera matrices) to trasform vertex into 2D clip space, pass to pixel shader
    output.clipPosition = mul(gViewProjectionMatrix, worldPosition);
    
    // Pass texture coordinates (UVs) traight to pixel shader
    output.uv = modelVertex.uv;
    
    return output; // Output data set down the pipeline
}