//====================================================================
// Pixel Shader - Diffuse Colour
//====================================================================
// Renders in a single colours from the material colour and sub-mesh

#include "Common.hlsli"

//====================
// Pixel Shader Code
//====================

float4 main() : SV_TARGET // Output is a float4 RGBA - which has a special semantic SV_Target to indicate it goes to the render target
{
	// Combines colour and alpha value from material diffuse colour and per-mesh colour
    return gMaterialDiffuseColour * gMeshColour;
}