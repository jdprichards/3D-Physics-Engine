//==========================================================
// RenderMethod-related types and the RenderState class
//==========================================================
// Render methods describe the process required to render a given material
// A render state is the DirectX objects and states that need to be set to support a given render method
//
// Each submesh in a mesh holds a single RenderState, a combination of vertex shader, pixel shader, textures and constants.
// Enable a particular RenderState object on the GPU by calling its Apply method, which will select shaders, attach textures
// to them etc. SUbsequent rendering will not use the appropriate render state.
//
// RenderState objects are constructed from RenderMethod objects. A RenderMethod is a readable descriptionof the processes 
// and textures / constants needed to render a material. RenderMethods are created during the import process (see Mesh.h/cpp)
// by looking at the materials used by submeshes in the 3D files

#include "RenderMethod.h"

#include "Shader.h"
#include "Texture.h"
#include "CBuffer.h"
#include "RenderGlobals.h"

#include <stdexcept>
#include <filesystem>
//=================
// Construction
//=================


// Construct GPU render states directly from RenderMethod descriptor
RenderState::RenderState(const RenderMethod& renderMethod)
{
	// Get shaders required to support given render method
	std::string vertexShaderName, pixelShaderName;

	if (renderMethod.geometryRenderMethod == GeometryRenderMethod::Rigid) // Vertex and Pixel shader name for each surface render method, assuming GeometryRenderMethod::Rigid
	{
		switch (renderMethod.surfaceRenderMethod)
		{
		case SurfaceRenderMethod::UnlitColour:			vertexShaderName = "vs_clipspace";						pixelShaderName = "ps_colour";							break;
		case SurfaceRenderMethod::UnlitTexture:			vertexShaderName = "vs_uv";								pixelShaderName = "ps_texture";							break;
		case SurfaceRenderMethod::BlinnColour:			vertexShaderName = "vs_position_normal";				pixelShaderName = "ps_blinn";							break;
		case SurfaceRenderMethod::BlinnTexture:			vertexShaderName = "vs_position_normal_uv";				pixelShaderName = "ps_blinn_ds";						break;
		case SurfaceRenderMethod::BlinnNormalMapping:	vertexShaderName = "vs_position_normal_tangent_uv";		pixelShaderName = "ps_blinn_normal";					break;
		case SurfaceRenderMethod::BlinnParallaxMapping: vertexShaderName = "vs_position_normal_tangent_uv";		pixelShaderName = "ps_blinn_parallax";					break;

		default:
			throw std::runtime_error("RenderState: Unsupported surface render method");
		}
	}

	else
		throw std::runtime_error("RenderState: Unsupported Render Method");

	vertexShaderName ="Render\\Compiled Shaders\\" + vertexShaderName;

	// Load required shaders (shader manager will ensure the same shader isn't loaded twice)
	mVertexShader = DX->Shaders()->LoadVertexShader(vertexShaderName);
	if (mVertexShader == nullptr)
		throw std::runtime_error("RenderState: " + DX->Shaders()->GetLastError());

	pixelShaderName = "Render\\Compiled Shaders\\" + pixelShaderName;
	mPixelShader = DX->Shaders()->LoadPixelShader(pixelShaderName);
	if (mPixelShader == nullptr)
		throw std::runtime_error("RenderState: " + DX->Shaders()->GetLastError());

	// Load all textures and samplers indicated by the render method. They are loaded into the array slot indicated b
	// the texture type and will appear in shaders on that same slot
	
	mTextures.fill(nullptr);
	mSamplers.fill(nullptr);

	for (int i = 0; i < renderMethod.textures.size(); i++)
	{
		// Support null textures (filename = ""). Set nullptr as texture, but set sampler as normal
		// DirectX debugger issues warning if there are null samplers
		if (renderMethod.textures[i].fileName != "")
		{
			bool allowSRGB =
				renderMethod.textures[i].type != TextureType::Roughness &&
				renderMethod.textures[i].type != TextureType::Normal &&
				renderMethod.textures[i].type != TextureType::Displacement;

			auto [texture, textureSRV] = DX->Textures()->LoadTexture(renderMethod.textures[i].fileName, allowSRGB);
			if (texture == nullptr)
				throw std::runtime_error("RenderState: Failed to load texture: " + renderMethod.textures[i].fileName + " - " + DX->Textures()->GetLastError());

			mTextures[static_cast<int>(renderMethod.textures[i].type)] = textureSRV;
		}
		else
		{
			mTextures[static_cast<int>(renderMethod.textures[i].type)] = nullptr;
		}

		auto sampler = DX->Textures()->CreateSampler(renderMethod.textures[i].samplerState);
		if (sampler == nullptr)
			throw std::runtime_error("RenderState: Failed to load sampler - " + DX->Textures()->GetLastError());

		mSamplers[static_cast<int>(renderMethod.textures[i].type)] = sampler;
	}

	// Initialise CPU-side constants
	mConstants = renderMethod.constants;
}

//=========
// Usage
//=========

// Srt up the GPU to use this render state
void RenderState::Apply()
{
	// Set each shader on GPU, don't do anything if currently selected shaderr is already the correct one
	// Note: the ShaderManager ensures that different meshes using the same shader get the same shader objexts so this will work across different meshes

	if (mVertexShader != mCurrentVertexShader)
	{
		DX->Context()->VSSetShader(mVertexShader, nullptr, 0);
		mCurrentVertexShader = mVertexShader;
	}
	if (mPixelShader != mCurrentPixelShader)
	{
		DX->Context()->PSSetShader(mPixelShader, nullptr, 0);
		mCurrentPixelShader = mPixelShader;
	}

	// Set textures and samplers on GPU, don't change the if current one are already correct
	for (int i = 0; i < mTextures.size(); i++)
	{
		if (mTextures[i] != mCurrentTextures[i])
		{
			DX->Context()->PSSetShaderResources(i, 1, &mTextures[i]);
			mCurrentTextures[i] = mTextures[i];
		}
	}

	for (int i = 0; i < mSamplers.size(); i++)
	{
		if (mSamplers[i] != mCurrentSamplers[i])
		{
			DX->Context()->PSSetSamplers(i, 1, &mSamplers[i]);
			mCurrentSamplers[i] = mSamplers[i];
		}
	}

	DX->CBuffers()->UpdateCBuffer(gPerMaterialConstantBuffer, mConstants);
}

// Set an environemnt map for all RenderStates
void RenderState::SetEnvironmentMap(ID3D11ShaderResourceView* environmentMap)
{
	if (mCurrentEnvironmentMap == environmentMap)
		return;

	DX->Context()->PSSetShaderResources(static_cast<int>(TextureType::Enviroment), 1, &environmentMap);
	mCurrentEnvironmentMap = environmentMap;
}

// CAll if DirectX state may have changed by a 3rd party librarry call
void RenderState::Reset()
{
	mCurrentVertexShader = {};
	mCurrentPixelShader = {};
	mCurrentTextures.fill(nullptr);
	mCurrentSamplers.fill(nullptr);
	mCurrentPixelShader = {};
}

//======================
// Static Private Data
//======================
//
// Shaders, states, textures, samplers already set on the GPU
// Use these vales to avoid requests to the GPU when the current GPU setting is already correct
// These are static members of the RenderState class (class global). Statics need to be initalised outside the class, here in the cpp file
ID3D11VertexShader* RenderState::mCurrentVertexShader = {};
ID3D11PixelShader* RenderState::mCurrentPixelShader = {};

std::array<ID3D11ShaderResourceView*, NUM_TEXTURE_TYPES> RenderState::mCurrentTextures = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
std::array<ID3D11SamplerState*, NUM_TEXTURE_TYPES> RenderState::mCurrentSamplers = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

ID3D11ShaderResourceView* RenderState::mCurrentEnvironmentMap = {};