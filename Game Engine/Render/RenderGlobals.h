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


#ifndef _RENDER_GLOBALS_H_INCLUDED_
#define _RENDER_GLOBALS_H_INCLUDED_

#include "DXDevice.h"
#include "CBufferTypes.h"

#include <memory>

//=======================
// DirectX Device
//=======================
// DX encapsulates the import D3Device and D3DContext objects used for almost all DirectX call
// as well as a few other useful global DirectX data as a backbuffer size

extern std::unique_ptr<DXDevice> DX;

//====================
// Constant Buffer
//====================
// Global Buffer for each of the constant structures defined in CBufferTypes.h

extern PerFrameConstants gPerFrameConstants; // The constants (settings) sent to the GPU each frame
extern ID3D11Buffer* gPerFrameConstantBuffer; //The GPU buffer that will recieve per-frame constants


extern PerCameraConstants gPerCameraConstants; // constants sent to GPU for each camera
extern ID3D11Buffer* gPerCameraConstantBuffer;

extern PerMeshConstants gPerMeshConstants; // constants sent to GPU for each mesh
extern ID3D11Buffer* gPerMeshConstantBuffer;

// There's also per-material constants which chnage to fit the material in a submesh
// however these are held in the MeshRenderState
extern ID3D11Buffer* gPerMaterialConstantBuffer;

// Creates all the global constant buffers. Return true on success.
bool CreateCBuffers();


#endif // !_RENDER_GLOBALS_H_INCLUDED_

