//===============================================================================================================
// Pixel Shader - Blinn-Phong - Diffuse Specular Mapping
//===============================================================================================================
// Blinn-Phong lighting with multiple lights and simple texture mapping. Uses diffuse and optional specular maps
// Combines with diffuse / specular colour settings from per-material constant buffer

#include "Common.hlsli"

//============
// Textures
//============

// Textures - order of indexes (t0, t1, etc.) is specified in MeshTypes.h : Texture Types
Texture2D DiffuseMap : register(t0);
Texture2D SpecularMap : register(t1);

// Samplers used for above textures
SamplerState DiffuseFilter : register(s0);
SamplerState SpecularFilter : register(s1);

//======================
// Pixel Shader Input
//======================

// Data coming in from vertex shader
struct Input
{
    float4 clipPosition : SV_Position;      // 2D position of pixel in clip space - this key field is identified by special semantic "SV_Position" (SV = System Value)
    float3 worldPosition : worldPosition;   // 3D position of pixelin world space - used for lighting
    float3 worldNormal : worldNormal;       // The surface normal ( in world space ) for this pixel - used for lighting
    float2 uv : uv;                         // The texture coordinate for this pixel,  used to sample textures
};

//====================
// Pixel Shader Code
//====================

// Pixel shader gets input from vertex shader output
float4 main(Input input) : SV_TARGET // Output is a float4 RGBA - which has a special semantic SV_TARGET to indicate it goes to the render target
{
    // Renormalize pixel normal and tangent because interpolation from vertex shader can introduce scaling
    float3 worldNormal = normalize(input.worldNormal);
    
    float3 finalDiffuseColour = { 0, 0, 0 };
    float3 finalSpecularColour = { 0, 0, 0 };
    
    // Blinn-Phong lighting for all lights in scene
    int i = 0;
    
    float3 lightVector = lights[i].gPosition - input.worldPosition;
    float lightDistance = length(lightVector);
    float3 lightNormal = lightVector / lightDistance;
        
    float3 cameraNormal = normalize(gCameraPosition - input.worldPosition);
    float3 halfwayNormal = normalize(cameraNormal + lightNormal);
        
        // Attenuated Light Colour (reduce strength based on distance)
    float3 attenuatedLightColour = lights[i].gColour / lightDistance;;
        
        // Diffuse Lighting
    float lightDiffuseLevel = saturate(dot(worldNormal, lightNormal));
    float3 lightDiffuseColour = attenuatedLightColour * lightDiffuseLevel;
        
        // Specular Lighting
    float lightSpecularLevel = pow(saturate(dot(worldNormal, halfwayNormal)), gMaterialSpecularPower);
    float3 lightSpecularColour = lightDiffuseColour * lightSpecularLevel; // Use diffuse light colour rather than plain attenuated colour
    
    finalDiffuseColour += lightDiffuseColour;
    finalSpecularColour += lightSpecularColour;
    
    
    // Material colours are taken from material colours combined with texture colours
    // Diffuse colour combines alpha from material, texture and mesh colour, and the result alpha will be the alpha of the rendered pixel
    float4 materialDiffuseColour = gMaterialDiffuseColour * DiffuseMap.Sample(DiffuseFilter, input.uv) * gMeshColour;  // Allow per-mesh tint
    float3 materialSpecularColour = gMaterialSpecularColour;
    
    // Combine all colours, adding ambient light as part of the process
    float3 finalColour = materialDiffuseColour.rgb * (gAmbientColour + finalDiffuseColour) + materialSpecularColour * finalSpecularColour;
    
    return float4(finalColour, materialDiffuseColour.a);
}