//=====================================
// DirectX Related Global Variables
//=====================================
// Includes a few globals to simplify the code architecture
// 
// The single DirectX object (DX) is used for all DirectX calls and used in most classes
// so insead of passing a pointer to all objects that deal with DirextX a single global is made
//
// The constant buffers (per-frame, per-camera, etc.) are used in various places within the project,
// in both the rendering and scene code. These are made global to avoid passing pointers to objects
//
// It's possible to have "singleton" classes which only contain a single object.
// But it will only make the code a tiny bit cleaner with minimal benefit.

#include "RenderGlobals.h"
#include "CBuffer.h"

//=======================
// DirectX Device
//=======================
// DX encapsulates the import D3Device and D3DContext objects used for almost all DirectX call
// as well as a few other useful global DirectX data as a backbuffer size

std::unique_ptr<DXDevice> DX;

//====================
// Constant Buffer
//====================
// Global Buffer for each of the constant structures defined in CBufferTypes.h

PerFrameConstants gPerFrameConstants; // The constants (settings) sent to the GPU each frame
ID3D11Buffer* gPerFrameConstantBuffer; //The GPU buffer that will recieve per-frame constants

PerCameraConstants gPerCameraConstants; // constants sent to GPU for each camera
ID3D11Buffer* gPerCameraConstantBuffer;

PerMeshConstants gPerMeshConstants; // constants sent to GPU for each mesh
ID3D11Buffer* gPerMeshConstantBuffer;

// There's also per-material constants which chnage to fit the material in a submesh
// however these are held in the MeshRenderState

ID3D11Buffer* gPerMaterialConstantBuffer;

//====================================
// Global Constant Buffer Creation
//====================================

// Creates all the global constant buffers. Return true on success.
bool CreateCBuffers()
{
	// Create GPU-side constant buffers to receive their corresponding constant structures above
	// Allow us to pass data from CPU to shades, such as lighting information or matrices
	
	gPerFrameConstantBuffer = DX->CBuffers()->CreateCBuffer(sizeof(PerFrameConstants));
	gPerCameraConstantBuffer = DX->CBuffers()->CreateCBuffer(sizeof(PerCameraConstants));
	gPerMaterialConstantBuffer = DX->CBuffers()->CreateCBuffer(sizeof(PerMaterialConstants));
	gPerMeshConstantBuffer = DX->CBuffers()->CreateCBuffer(sizeof(PerMeshConstants));

	if (gPerFrameConstantBuffer == nullptr || gPerCameraConstantBuffer == nullptr ||
		gPerMaterialConstantBuffer == nullptr || gPerMeshConstantBuffer == nullptr)
		return false;

	// Enable these constant buffers on all shaders.
	// Each one is given a slot number that should match number in constant buffer decleration.
	// Buffer decleration in shaders (see Common.hlsli) e.g. bufferName : register(b?)
	DX->CBuffers()->EnableCBuffer(gPerFrameConstantBuffer, 0);
	DX->CBuffers()->EnableCBuffer(gPerCameraConstantBuffer, 1);
	DX->CBuffers()->EnableCBuffer(gPerMeshConstantBuffer, 2);
	DX->CBuffers()->EnableCBuffer(gPerMaterialConstantBuffer, 3);

	return true;
}