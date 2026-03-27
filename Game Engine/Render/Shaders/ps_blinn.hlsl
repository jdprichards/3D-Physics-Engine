//==============================================================================================
// Pixel Shader - Blinn-Phong - No Textures
//==============================================================================================
// Blinn-Phong lighting with mutliple lights and pain colours (no textures) taken from
// diffuse / specular colour settings from per-material constant buffer

#include "Common.hlsli"

//=======================
// Pixel Shader Input
//=======================

// Data coming in from vertex shader
struct Input
{
    float4 clipPosition : SV_Position;      // 2D position of pixel in clip space - Key field is identified by special semantics "SV_Position" (SV = System Value)
    float3 worldPosition : worldPosition;   // 3D position of pixel in world space - used for lighting
    float3 worldNormal : worldNormal;       // The surface normal (in world space) for current pixel - used for lighting
};

//====================
// Pixel Shader Code
//====================

// Pixel shader gets input from vertex shader output (above structure)
float4 main(Input input) : SV_TARGET // Output is float4 RGBA - which has a special semantic SV_TARGET to indicate it goes on the render target
{
    // Renormalize the pixel normal - due to interpolation from the pixel shader (can introduce scaling)
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
    float3 attenuatedLightColour = lights[i].gColour / lightDistance;
        
        // Diffuse Lighting
    float lightDiffuseLevel = saturate(dot(worldNormal, lightNormal));
    float3 lightDiffuseColour = attenuatedLightColour * lightDiffuseLevel;
        
        // Specular Lighting
    float lightSpecularLevel = pow(saturate(dot(worldNormal, halfwayNormal)), gMaterialSpecularPower);
    float3 lightSpecularColour = lightDiffuseColour * lightSpecularLevel; // Use diffuse light colour rather than plain attenuated colour
    
    finalDiffuseColour += lightDiffuseColour;
    finalSpecularColour += lightSpecularColour;
    
    
    float4 materialDiffuseColour = gMaterialDiffuseColour * gMeshColour;
    float3 finalColour = materialDiffuseColour.rgb * (gAmbientColour + finalDiffuseColour) + gMaterialSpecularColour * finalSpecularColour;
    
    return float4(finalColour, materialDiffuseColour.a);
}