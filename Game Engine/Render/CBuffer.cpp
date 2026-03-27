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

#include "CBuffer.h"

//=============================
// Constant Buffer Creation
//=============================

// Create a constant buffer ofgiven size (in bytes). Typically pass size of the struct that the C++
// code will use for the content of the constant buffer. Returns a DirectX constnat buffer object
// or nullptr on error. Do not releaswe the returned pointer as the CBufferManager object manages buffer
// lifetimes. If nullptr is returned, you can call GetLastError() for a string description of the error.
ID3D11Buffer* CBufferManager::CreateCBuffer(std::size_t size)
{
	// Constant buffer settings
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.ByteWidth = static_cast<UINT>(16 * ((size + 15) / 16)); // Constant buffer size must be a mutiple of 16 - this maths rounds up to the nearest multiple
	cbDesc.Usage = D3D11_USAGE_DYNAMIC; // Indicates that the bufferis frequently updated
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPU is only going to write to the constants
	cbDesc.MiscFlags = 0;

	// Create the buffer
	CComPtr<ID3D11Buffer> cBuffer;
	HRESULT hr = mDXDevice->CreateBuffer(&cbDesc, nullptr, &cBuffer);
	if (FAILED(hr))
	{
		mLastError = "Error creating constant buffer";
		return nullptr;
	}

	mCBuffers.push_back(cBuffer);
	return cBuffer;
}

//==================================
// Constant Buffer Help Functions
//==================================

// Enable the given constant buffer on the given slot for all shaders - i.e. all shaders will get access to it
// Pass the DirectX constant buffer object and the slot index to use it on. The matching constnat buffer declaration
// in the HLSL code should be declared:
// cbuffer bufferName : register(b?) where ? is the slot number
void CBufferManager::EnableCBuffer(ID3D11Buffer* buffer, unsigned int slot)
{
	mDXContext->VSSetConstantBuffers(slot, 1, &buffer);
	mDXContext->HSSetConstantBuffers(slot, 1, &buffer);
	mDXContext->DSSetConstantBuffers(slot, 1, &buffer);
	mDXContext->GSSetConstantBuffers(slot, 1, &buffer);
	mDXContext->PSSetConstantBuffers(slot, 1, &buffer);

}