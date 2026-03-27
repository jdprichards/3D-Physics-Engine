//==============================
// Texture Related Types
//==============================

#ifndef _TEXTURE_TYPES_H_INCLUDED_
#define _TEXTURE_TYPES_H_INCLUDED_

#include <string>

//==================
// Textures
//==================

// There are four rendering approaches supported here, each needing different texture types:
// - Unlit rendering - plain colour objects (all one colour) or plain texture objects (e.g. skybox - doesn't receive lighting)
// - Blinn-Phong rendering (Simple lighting equations for diffuse + specular lighting)
// - Physically-Based-Rendering (PBR) with metalness / roughness textures
// - Physically-Based-Rendering (RBR) with specular / glossiness textures
//
// Each Texture type is given a specific slot in the shaders: the values in the enum below
// Slot have been arranged so different rendering approaches above share slot whrere possible
// Not supporting situtations where two maps are stored in a single texture (e.g. diffuse + specular map, normal + height map, metalness + roughness map),
// so each texture must be standalone in this app
enum class TextureType
{
	// Blinn-Phong rendering approach
	Diffuse = 0, // Basic colour of object, affected by diffuse light, also used for plain texture rendering
	Specular = 1, // Whininess of object, affected by specular light
	SpecularPower = 2, // Exponent used in specular equations
	Normal = 3, // For normal mapping
	Displacement = 4, // aka Height, for parallax mapping or tesellation displacement

	// Physically-Based Rendering (metalness / roughness approach)
	Albedo = 0, // Base colour of object
	Metalness = 1, // 0 = non-metal, 1 = metal, values inbetween supported
	Roughness = 2, // 0 = smooth to 1 = though. Same as (1 - Gross map)
	// Slot 3 and 4 will be normal and displacement

	// Physically-Based-Rendering (specular / glossiness approach)
	// Slot 0 will be albendo / diffuse
	// Slot 1 will be specular
	Gloss = 2, // 0 = rough to 1 = smooth. Same as (1 - Roughness map)
	// Slot 3 and 4 will be normal and displacement

	// Optional maps
	AO = 5, // Ambient occlusion map - 0->1 values, recessed areas get lower values, affects ambient light
	Cavity = 6, // Cavity map - 0->1 values, highly recessed areas get lower values, affects direct light
	Emissive = 6, // Emissive map is colour *emitted* by object. Added to final calculated colour value

	Enviroment = 8, // Environment map is reflected in shiny surfaces in Physically-Based-Rendering, should be a cude map

	Unkown,
};

const int NUM_TEXTURE_TYPES = 8; // Total number of slots unded in enum above - maximun number of shader slot that will be used in shaders


//==================
// Samplers
//==================


// How to filter large / small textures
enum class TextureFilter
{
	FilterPoint,
	FilterBilinear,
	FilterTrilinear,
	FilterAnisotropic,
};

// How to access UVs outside the 0->1 range
enum class TextureAddressingMode
{
	AddressingWrap,
	AddressingMirror,
	AddressingClamp,
	AddressingBorder,
};

// A sampler describes how a specific texture is accessed in a shader. Where a texture is used in a 
// shader, a sampler must also have been set to go with it
struct SamplerState
{
	TextureFilter filter;
	TextureAddressingMode addressingMode;
};


// Less-than operator for SamplerState objects
// The TextureManager class uses a map of SamplerStates to DirectX samplers. However, std::map requires that
// the key (SamplerState) has a less then operator so we define one here so we can use it as the key in a map
inline bool operator<(const SamplerState& a, const SamplerState& b)
{
	if (a.filter < b.filter)
		return true;
	else if (a.filter > b.filter)
		return false;
	else
		return (a.addressingMode < b.addressingMode);
}


//===============
// Other
//===============

// Collected data to describe a texture used in a submesh
struct TextureDesc
{
	TextureType type = TextureType::Unkown;
	std::string fileName;
	SamplerState samplerState;
};

#endif // !_TEXTURE_TYPES_H_INCLUDED_
