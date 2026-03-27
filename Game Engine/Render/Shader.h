//==============================================================================
// ShaderManager class encapsulates the loading and destruction of Shaders
//==============================================================================
// Other code that requires a shader must pass the shader fullname to the ShaderManager, if the ShaderManager has already
// loaded that shader the existing pointer is returned, otherwise the shader is loaded. The calling code is not responsible
// for the lifetime ofo the shader and does not need to release it ever. The ShaderManager will release all shaders when
// it is destroyed. Practically that means all shaders exist until the app closes but that is fine as they're small.

#ifndef _SHADER_H_INCLUDED_
#define _SHADER_H_INCLUDED_

#define NOMINMAX // Use NOMINMAX to stop certain Microsoft headers defining "min" and "max", which breaks some libraries
#include <d3d11.h>
#include <atlbase.h> // For CComPtr

#include <map>
#include <vector>
#include <string>

class ShaderManager
{
public:
	//===================
	// Construction
	//===================
	// Create the texture manager, pass DiretX device and context
	ShaderManager(ID3D11Device* device, ID3D11DeviceContext* context)
		: mDXDevice(device), mDXContext(context) {}

public:
	//========================
	// Shader Creation
	//========================
	
	// Load shader with given filename. Do not include the extension in the filename
	// Returns a DirectX shader object for use in d3dContext->?SetShader. Returns a nullptr on error
	// Do not release the returned pointer as the ShaderManager object manages the shader object lifetime.
	// If nullptr is returned, you can call GetLastError() for a string description of the error
	// The function reads the compiled .cso file not the .hlsl file. All shader names should be unique
	// Shadermanager stores previously loaded shaders and will return them if a shader is requested for a second time.
	ID3D11VertexShader* LoadVertexShader(std::string shaderName);
	ID3D11HullShader* LoadHullShader(std::string shaderName);
	ID3D11DomainShader* LoadDomainShader(std::string shaderName);
	ID3D11GeometryShader* LoadGeometryShader(std::string shaderName);
	ID3D11PixelShader* LoadPixelShader(std::string shaderName);

	// Special method to load a geometry shader that can use the stream-out stage, Use like other shader loading functions
	// except also pass the stream out declaration, number of entries in the declaration and the total size of the output data
	// Stores the shader name and returns the same shader pointer if the same name is requested again - for that reason
	// you should not use the same shader name with different stream-out specifications
	ID3D11GeometryShader* LoadStreamOutGeometryShader(std::string shaderName, D3D11_SO_DECLARATION_ENTRY* soDecl,
		unsigned int soNumEntries, unsigned int soStride);

	// If LoadXXShader functions return nullptr to indicate an error, the text description 
	// of the (most recent) error can be fetched with this function
	std::string GetLastError() { return mLastError; }

private:
	//======================
	// Helper Functions
	//======================

	// Load the cso (compiled shader object) file of a given shader (any type) and put the bytecode in the 
	// given vector. Used by all shader loading functions above. Returns true and sucess
	bool LoadShaderByteCode(std::string shaderName, std::vector<char>& byteCode);

private:
	//======================
	// Private Data
	//======================

	ID3D11Device* mDXDevice;
	ID3D11DeviceContext* mDXContext;

	// Maps of shader filenames to DirectX shader objects. Given a new shader filename to load, can quickly
	// check to see if it has already been loaded and if so return the existing pointer.
	// CComPtr is a smart pointer that can be used for DirectX objects (similar to unique_ptr). Using CComPtr
	// means we don't need a destructor to release everything from DirectX, it will happen automatically when
	// the ShaderManager is destroyed
	// CComPtr requires <atlbase.h> to be included (and the ATL component to be installed as part of Visual Studio)
	std::map<std::string, CComPtr<ID3D11VertexShader>> mVertexShaders;
	std::map<std::string, CComPtr<ID3D11HullShader>> mHullShaders;
	std::map<std::string, CComPtr<ID3D11DomainShader>> mDomainShaders;
	std::map<std::string, CComPtr<ID3D11GeometryShader>> mGeometryShaders;
	std::map<std::string, CComPtr<ID3D11PixelShader>> mPixelShaders;

	// Description of the most recent error from LoadXXShader function
	std::string mLastError;


};

//======================
// Helper Functions
//======================

// Helper function that creates a shader signature to assist in the creation of vertex layouts for vertex buffers
// Returned object should be released when it has been finished with. Returns nullptr on failure
ID3DBlob* CreateSignatureForVertexLayout(const D3D11_INPUT_ELEMENT_DESC vertexLayout[], int numElements);

#endif // !_SHADER_H_INCLUDED_
