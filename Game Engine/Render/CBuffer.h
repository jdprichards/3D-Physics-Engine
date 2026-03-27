//===============================================================================
// CBufferManager encapsulates the creation / destruction of constant buffers
//===============================================================================
// Some general constant buffer support functions are here
//
// Constant buffers help pass data from C++ to support shaders. Most commonly matrices and light information,
// but also anything required aside from the vertex buffer data. Constant buffersappear as small structs, with
// matching definitions in C++ (CPU) and HLSL (GPU). The CBufferManager helps create, maintain and destroy
// constant buffers. Any code that requires a constant buffer should request it from this call and the calling
// code will not be repsonsible for the lifetime of the returned buffer. The CBufferManager will release all
// its buffers when destroyed. Practically that means that all constant buffers will exist until the app closes.

#ifndef _C_BUFFER_H_INCLUDED_
#define _C_BUFFER_H_INCLUDED_

#define NOMINMAX // Use NOMINMAX to stop certain Miscrosoft headers defining "min" and "max" which breaks some libraries
#include <d3d11.h>
#include <atlbase.h> // For CComPtr

#include <vector>
#include <string>
#include <cstddef>

//===================================
// Constant Buffer Manager Class
//===================================
class CBufferManager
{
public:
	//==================
	// Construction
	//==================
	
	// Create the constant buffer manager, pass DirectX device and context
	CBufferManager(ID3D11Device* device, ID3D11DeviceContext* context)
		: mDXDevice(device), mDXContext(context) {}

public:
	//=============================
	// Constant Buffer Creation
	//=============================

	// Create a constant buffer ofgiven size (in bytes). Typically pass size of the struct that the C++
	// code will use for the content of the constant buffer. Returns a DirectX constnat buffer object
	// or nullptr on error. Do not releaswe the returned pointer as the CBufferManager object manages buffer
	// lifetimes. If nullptr is returned, you can call GetLastError() for a string description of the error.
	ID3D11Buffer* CreateCBuffer(std::size_t bufferSize);

	// If CreateCBuffer function returns nullptr to indicate an error, the text description
	// of the (most recent) error can be fetched with this function.
	std::string GetLastError() { return mLastError; }

	//====================
	// Helper Functions
	//====================

	// Enable the given constant buffer on the given slot for all shaders - i.e. all shaders will get access to it
	// Pass the DirectX constant buffer object and the slot index to use it on. The matching constnat buffer declaration
	// in the HLSL code should be declared:
	// cbuffer bufferName : register(b?) where ? is the slot number
	void EnableCBuffer(ID3D11Buffer* buffer, unsigned int slot);

	// Template function to update the constant buffer. Pass the DirectX constant buffer object and the C++ data structure
	// you want to update it with. The structure will be copied in full over to the GPU constant buffer, where it will
	// be avaliable to shaders. This is used to update model and camera position, lighting data, etc.
	template <class T>
	void UpdateCBuffer(ID3D11Buffer* buffer, const T& bufferData)
	{
		D3D11_MAPPED_SUBRESOURCE cb;
		mDXContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &cb);
		memcpy(cb.pData, &bufferData, sizeof(T));
		mDXContext->Unmap(buffer, 0);

	}

private:
	//================
	// Private Data
	//================

	ID3D11Device* mDXDevice;
	ID3D11DeviceContext* mDXContext;

	// Vector of DirectX constant buffer objects
	// CComPtr is a smart pointer that can be used for DirectX objects (similar to unique_ptr). Using CComPtr means we
	// don't need a destructor to release everything from DirectX, it will happen automatically when the CBufferManager
	// is destroyed. CComPtr requires <atlbase.h> to be included
	std::vector<CComPtr<ID3D11Buffer>> mCBuffers;

	// Description of thje most recent error from CreateCBuffer
	std::string mLastError;
};


#endif // !_C_BUFFER_H_INCLUDED_

