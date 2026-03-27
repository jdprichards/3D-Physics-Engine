//================================================================================================================================
// Pixel Shader - Blinn-Phong - Parallax Mapping
//================================================================================================================================
// Blinn-Phong lighting with multiple lights and parallax mapping. Uses diffuse, normal, displacement and optional specular maps
// Combines with diffuse / specular colour settings from per-material constant buffer

#include "Common.hlsli"

//============
// Textures
//============

// Textures - order of indexes (t0, t1, etc.) is specified in MeshTypes.h : Texture Types
Texture2D DiffuseMap : register(t0);
Texture2D SpecularMap : register(t1);
Texture2D NormalMap : register(t2);
Texture2D DisplacementMap : register(t3);

// Samplers used for above textures
SamplerState DiffuseFilter : register(s0);
SamplerState SpecularFilter : register(s1);
SamplerState NormalFilter : register(s2);
SamplerState DisplacementFilter : register(s3);

//======================
// Pixel Shader Input
//======================

// Data coming in from vertex shader
struct Input
{
    float4 clipPosition : SV_Position;      // 2D position of pixel in clip space - this key field is identified by special semantic "SV_Position" (SV = System Value)
    float3 worldPosition : worldPosition;   // 3D position of pixelin world space - used for lighting
    float3 worldNormal : worldNormal;       // The surface normal ( in world space ) for this pixel - used for lighting
    float3 worldTangent : worldTangent;     // The surface tangent ( in world space ) for this pixel - used for normal / parallax mapping
    float2 uv : uv;                         // The texture coordinate for this pixel,  used to sample textures
};

//====================
// Pixel Shader Code
//====================

// Pixel shader gets input from pixel shader output
float4 main(Input input) : SV_TARGET // Output is a float4 RGBA - which has a special SV_Target to indicate it goes to the render target
{
    // Renormalize pixel normal and tangent because interpolation from the vertex shader can introduce scaling
    float3 worldNormal = normalize(input.worldNormal);
    float3 worldTangent = normalize(input.worldTangent);
    
    // Create the bitangent - at right angles to normal and tangent. Then with these three axes, form the tangent matrix - the local axes for the current pixel
    float3 worldBitangent = cross(worldNormal, worldTangent);
    float3x3 tangentMatrix = float3x3(worldTangent, worldBitangent, worldNormal);
    
    // Get displacement at current pixel, convert 0 -> 1 range to -0.5 -> 0.5 range
    float displacement = DisplacementMap.Sample(DisplacementFilter, input.uv).r - 0.5f;
    
    // Parallax mapping
    float3 cameraNormal = normalize(gCameraPosition - input.worldPosition);     // Get normal to camera
    float3 parallaxOffset = mul(tangentMatrix, cameraNormal);                   // Transform camera direction into tangent space (so it is local to the texture)
    float2 uv = input.uv + gParallaxDepth * displacement * parallaxOffset.xy;   // Offset texture uv in camera's view directon based on displacement
                                                                                // Can divide this by parallaxOffset.z to remove limiting (stronger effect, prone to artefacts)
    
    // Sample the normal map - including conversion from 0 -> 1 RGBvalues to -1 -> 1 XYZ values
    float3 tangentNormal = normalize(NormalMap.Sample(NormalFilter, uv).xyz * 2.0f - 1.0f);
    
    // Convert the sampled normal from tangent space (as it was stored) into world space. Now replaces the world normal for lighting
    worldNormal = mul(tangentNormal, tangentMatrix);
    
    float3 finalDiffuseColour = { 0, 0, 0 };
    float3 finalSpecularColour = { 0, 0, 0 };
    
    // Blinn-Phong lighting for all lights in scene
    int i = 0;
        
    float3 lightVector = lights[i].gPosition - input.worldPosition;
    float lightDistance = length(lightVector);
    float3 lightNormal = lightVector / lightDistance;
        
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
    // Diffuse colour combines alpha from material, texture and mesh colours, and the result will be the alpha of the rendered pixel
    float4 materialDiffuseColour = gMaterialDiffuseColour * DiffuseMap.Sample(DiffuseFilter, uv) * gMeshColour;
    float3 materialSpecularColour = gMaterialSpecularColour;
    
    // Combine all colours, adding ambient light ss part of the process
    float3 finalColour = materialDiffuseColour.rgb * (gAmbientColour + finalDiffuseColour) + materialSpecularColour * finalSpecularColour;
    
	return float4(finalColour, materialDiffuseColour.a);
}