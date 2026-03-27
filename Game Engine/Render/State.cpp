//====================================================================================
// StateManager class encapsulates the handling of GPU pipeline states such as blending or culling
//====================================================================================
// GPU state handle various global settings that adjust how the GPU renders:
// - Rasterizer State (Wireframe mode, don't cull back faces, etc.)
// - Depth Stencil Stare (How to use the depth and stencil buffer)
// - Blender State (Additive Blending, alpha blending, etc.)
// Each state requires a DirectX object to be created in advance and set when required.
// The state manager calss entirely manages these DirectX objects (creation and cleanup)
// States can be set at render time with simple call specifying an enum value RenderState.

#include "State.h"

#include <stdexcept>

//===================
// Construction
//===================
// Create the state manager, pass DirectX device and context
StateManager::StateManager(ID3D11Device* device, ID3D11DeviceContext* context)
{
	mDXDevice = device;
	mDXContext = context;

	//=======================
	// Rasterizer States
	//=======================
	// Rasterizer states adjust how triangles are filled in and when they are shown 
	// Each block of code creates a new rasterizer state

	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	CComPtr <ID3D11RasterizerState> newRasterizerState;


	//======== Back Face Culling ========//
	// This is the usaul mode - don't show inside faces of objects
	
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK; // Remove back faces
	rasterizerDesc.DepthClipEnable = TRUE; // Advanced seting only used in rare cases

	// Create a DirectX object for the description above that can be used by a shader
	if (FAILED(mDXDevice->CreateRasterizerState(&rasterizerDesc, &newRasterizerState)))
		throw std::runtime_error("Error creating cull-back state");
	mRasterizerStates[RasterizerState::CullBack] = newRasterizerState;


	//======== Front Face Culling ========//
	// This is an unusual mode - it shows inside faces only so model looks like its inside out
	
	newRasterizerState = {}; // Need to null out CComPtr if we want to reuse the variable
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_FRONT; // Remove front faces
	rasterizerDesc.DepthClipEnable = TRUE;

	if (FAILED(mDXDevice->CreateRasterizerState(&rasterizerDesc, &newRasterizerState)))
		throw std::runtime_error("Error creating cull-front state");
	mRasterizerStates[RasterizerState::CullFront] = newRasterizerState;


	//======== No Culling ========//
	// Used for transparent or flat objects - shows both sides of faces

	newRasterizerState = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK; // Don't remove any faces
	rasterizerDesc.DepthClipEnable = TRUE;

	// Create a DirectX object for the description above that can be used by a shader
	if (FAILED(mDXDevice->CreateRasterizerState(&rasterizerDesc, &newRasterizerState)))
		throw std::runtime_error("Error creating cull-none state");
	mRasterizerStates[RasterizerState::CullNone] = newRasterizerState;


	//======== Wireframe Mode with Back Face Culling ========//
	newRasterizerState = {};
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthClipEnable = TRUE;

	if (FAILED(mDXDevice->CreateRasterizerState(&rasterizerDesc, &newRasterizerState)))
		throw std::runtime_error("Error creating Wireframe with Cull Back state");
	mRasterizerStates[RasterizerState::CullBackWireframe] = newRasterizerState;


	//======== Wireframe Mode with No Culling ========//
	// See-throught wireframe mode

	newRasterizerState = {};
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.DepthClipEnable = TRUE;
	
	if (FAILED(mDXDevice->CreateRasterizerState(&rasterizerDesc, &newRasterizerState)))
		throw std::runtime_error("Error creating Wireframe with Cull None state");
	mRasterizerStates[RasterizerState::CullBackWireframe] = newRasterizerState;



	//===========================
	// Depth-Stencil States
	//===========================
	// Depth-stencil states adjust how the depth and stencil buffers are used. The stencil buffer is rarely used so
	// these states are most often used to switch the buffer on and off. 
	// Each block creates a new Depth-Stencil State

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	CComPtr<ID3D11DepthStencilState> newDepthState;

	//======== Enable Depth Buffer ========//
	// Enable depth buffer - default setting
	
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = FALSE;

	// Create a DirectX object for the description above that can be used by a shader
	if (FAILED(mDXDevice->CreateDepthStencilState(&depthStencilDesc, &newDepthState)))
		throw std::runtime_error("Error creating used-depth-buffer state");
	mDepthStates[DepthState::DepthOn] = newDepthState;


	//======== Enable Depth Buffer Read Only ========//
	// Diables writing to depth buffer - used for transparent objects because they would not be entered into the depth buffer but we do need to check if something is behind
	
	newDepthState = {};
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = FALSE;

	if (FAILED(mDXDevice->CreateDepthStencilState(&depthStencilDesc, &newDepthState)))
		throw std::runtime_error("Error creating depth-read-only state");
	mDepthStates[DepthState::DepthReadOnly] = newDepthState;


	//======== Disable Depth Buffer ========//
	// Entirely disable depth buffer, rendering will draw over everything

	newDepthState = {};
	depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthEnable = D3D11_COMPARISON_LESS;

	if (FAILED(mDXDevice->CreateDepthStencilState(&depthStencilDesc, &newDepthState)))
		throw std::runtime_error("Error creating no-depth-buffer state");
	mDepthStates[DepthState::DepthOff] = newDepthState;



	//=======================
	// Blending States
	//=======================
	// Blend states enable different kinds of transparency for render objects
	// Each block creates a blending mode

	D3D11_BLEND_DESC blendDesc = {};
	CComPtr<ID3D11BlendState> newBlendState;
	
	// Following setting should be left alone, they're only used in unusaul cases and will remain the same for the blending modes used here
	// Even though the word "Alpha" is used in variable names, these are not the settings for alpha blending.

	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;


	//======== No Blending State ========//
	blendDesc.RenderTarget[0].BlendEnable = FALSE; // Disable blending
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE; // How to blend the source (texture colour)
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO; // How to blend the destination (colour already on the screen)
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD; // How to combine the above two, almost always ADD

	if (FAILED(mDXDevice->CreateBlendState(&blendDesc, &newBlendState)))
		throw std::runtime_error("Error creating no-blend state");
	mBlendStates[BlendState::BlendNone] = newBlendState;


	//======== Additive Blending State ========//
	newBlendState = {};
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE; // ONE * the source (render colour)
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE; // ONE * the destination (colour already on screen)
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD; // ADD together, source + destination
	
	if (FAILED(mDXDevice->CreateBlendState(&blendDesc, &newBlendState)))
		throw std::runtime_error("Error creating additive blending state");
	mBlendStates[BlendState::BlendAdditive] = newBlendState;


	//======== Multiplicative Blending State ========//
	newBlendState = {};
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO; // ZERO * the source (rendered colour)
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR; // SRC_COLOUR * destination (colour already on screen)
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD; // Add together i.e. restult is 0 * src_colour + src_colour * dest_colour 
															// So just src_colour * dest_colour

	if (FAILED(mDXDevice->CreateBlendState(&blendDesc, &newBlendState)))
		throw std::runtime_error("Error creating multiplicative blending state");
	mBlendStates[BlendState::BlendMultiplicative] = newBlendState;


	//======== Alpha Blending State ========//
	newBlendState = {};
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; // SRC_ALPHA * source colour (rendered colour)
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; // (1 - SRC_ALPHA) * destination colour (colour already on screen)
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD; // Add together i.e. src_alpha * src_colour + (1 - src_alpha) * dest_colour. Alpha blending
	if (FAILED(mDXDevice->CreateBlendState(&blendDesc, &newBlendState)))
		throw std::runtime_error("Error creating alpha blending state");
	mBlendStates[BlendState::BlendAlpha] = newBlendState;
}

//===============
// Usage
//===============

// Enable the given rasterizer state. Return true on sucess. If it fails, use GetLastError to get a description of the error
bool StateManager::SetRasterizerState(RasterizerState state)
{
	if (mCurrentRasterizerState == state)
		return true;

	if (!mRasterizerStates.contains(state))
	{
		mLastError = "Cannot find requested rasterizer state";
		return false;
	}

	auto newState = mRasterizerStates[state];
	mDXContext->RSSetState(newState);
	mCurrentRasterizerState = state;
	return true;
}

// Enable the given depth state
bool StateManager::SetDepthState(DepthState state)
{
	if (mCurrentDepthState == state)
		return true;

	if (!mDepthStates.contains(state))
	{
		mLastError = "Cannot find requested depth state";
		return false;
	}

	auto newState = mDepthStates[state];
	mDXContext->OMSetDepthStencilState(newState, 0);
	mCurrentDepthState = state;
	return true;
}

// Enable the given blend state
bool StateManager::SetBlendState(BlendState state)
{
	if (mCurrentBlendState == state)
		return true;

	if (!mBlendStates.contains(state))
	{
		mLastError = "Cannot find requested blend state";
		return false;
	}

	auto newState = mBlendStates[state];
	mDXContext->OMSetBlendState(newState, nullptr, 0xffffffff);
	mCurrentBlendState = state;
	return true;
}

//===========================
// Static Private Data
//===========================

// States that are already set on the GPU
// Uses these values to avoid sending requests to the GPU whe the current GPU setting is correct
RasterizerState StateManager::mCurrentRasterizerState = RasterizerState::CullBack;
DepthState StateManager::mCurrentDepthState = DepthState::DepthOn;
BlendState StateManager::mCurrentBlendState = BlendState::BlendNone;