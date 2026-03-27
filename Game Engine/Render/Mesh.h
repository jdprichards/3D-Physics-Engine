//======================================================================
// Mesh Class Encapsulate Animated Geometry for DirectX Rendering
//======================================================================
// The mesh class splits the mesh into sub-meshes that only use one texture each.
// The class also doesn't load textures, filters or shaders as outer code is
// expected to select things

#ifndef _MESH_H_INCLUDED_
#define _MESH_H_INCLUDED_

#include "MeshTypes.h"
#include "RenderMethod.h"

#include "Matrix4x4.h"
#include "ColourTypes.h"
#include "Utility.h"

#define NOMINMAX

#include <d3d11.h>
#include <assimp/scene.h>
#include <atlbase.h> // For CComPtr

#include <filesystem>
#include <string>
#include <memory>

//=============
// Types
//=============
// Settings for mesh import via assimp - used in Mesh class constructor
// can be combined, e.g. auto flags = ImportFlgas::FixNormals | ImportFlags::DoNotLoadTextures;
enum class ImportFlags : uint32_t
{
	SimpleUVMapping = 0x1,				// Remap spherical, cubic mapping or similar to ordinary UV mapping. Apply any UV transformations so they are not required. This flag is required
	HierarchyFlags = 0x6,				// Parent for next 4 settings:
	RetainHierarchy = 0x0,					// All nodes are retained
	OptimiseHierarchy = 0x2,				// Nodes without meshes are removed, with the exception of the root node. Do not use if relying on dummy nodes
	FlatternHierarchyExceptBones = 0x4,		// Only nodes with bones are retained
	FlattenHierarchy = 0x6,					// Entire hierarchy is flattened to a single node
	UVaxisUp = 0x8,						// Set V axis of texture UVs to go up, rather than the default down
	RemoveLinesPoints = 0x10,			// Required setting, lines and points are not supported
	RemoveDegenerates = 0x20,			// Degenerate triangles or points are removed
	FixNormals = 0x40,					// Attempts to fix inverted normals
	SelectiveDebone = 0x80,				// Remove bones from meshes that are only affected by one bone (will be animated by node transforms only)
	NoLighting = 0x100,					// Do not use a lighting model for this mesh - will be plain colour or plain texture
	Validate = 0x200					// Additional validation and logging of import and post-processing
};
// Use above enum as flags (add bitwise operators)
ENUM_FLAG_OPERATORS(ImportFlags)


//================
// Mesh Class
//================

class Mesh
{
public:
	//================
	// Construction
	//================

	// Pass the name of the mesh file to load. Using assimp (https://github.com/assimp/assimp) to support many file types
	// Will throw a std::runtime_error excepion on failure (since constuctors can't return errors)
	// Optionally pass extra import filags over and above the default.
	// Avalible falgs here are:
	//	OptimiseHierarchy, FlatternHierarcchyExceptBones, FlattenHierarchy, UVAxisUp, SelectiveDebone and NoLighting
	Mesh(const std::string& fileName, ImportFlags additionalImportFlags = {});


	// Special Mesh constuctor to create a grid mesh without needing a file
	// Create grid in the XZ plane from pinPt to maxPt with the given number of subdivisions in X and Z
	// Optionally select wether to create normals (upwards), and / or UVs (0->1 square over the entire grid)
	// If UVs requsted optionally indicate how many repeats of the texture are required in X and Z (defaults to same as subdivisions)
	Mesh(Vector3 minPt, Vector3 maxPt, int subDivX, int subDivZ, bool normals = false, bool uvs = true, float uvRepeatX = 1, float uvRepeatY = 1);


public:
	//===============
	// Data Access
	//===============

	// How many nodes are in the hierarchy for this mesh. Nodes can control individual parts (rigid body animation),
	// or bones (skinned animation), or they can be dummy nodes to control child parts in a more convenient way
	unsigned int NodeCount() { return static_cast<unsigned int>(mNodes.size()); }
	
	unsigned int PolygonCount();
	std::vector<Vector3> Vertices();
	std::vector<Vector3> ScaledVertices(std::vector<Matrix4x4> transforms);

	// The default transformation matrix for a given mode - used to set the inital postion for a new model
	Matrix4x4 DefaultTrasformation(unsigned int node) { return mNodes[node].transfrom; }

	// Calculate the absolute matrix for given mode given a set of mesh transforms
	Matrix4x4 AbsoluteMatrix(const std::vector<Matrix4x4>& transforms, unsigned int node);


public:
	//=========
	// Usage
	//=========

	// Render the mesh with the given transforms
	// Hadles rigid body meshes (including single part meshes) as well as skinned meshes
	void Render(const std::vector<Matrix4x4>& transforms = {}, ColourRGBA colour = { 1, 1, 1, 1 });
	
private:
	//==========================
	// Private Data Structures
	//==========================

	// A mesh is a hierarchy of nodes. A node represents a sperate animatable part of the mesh
	// A node typically contains some geometry (but might not - a dummy node)
	// A node's geometry might be made of several sub-meshes - each sub-mesh using just a single material
	// A node can also have child nodes. The children will follow the motion of the parent node
	// Each node has a default transformation matrix which is it's initial / default, position / rotaiton / scale
	// Entities using this mesh are given these defualt matrices as a starting position
	struct Node
	{
		std::string name = "";

		Matrix4x4 transfrom = Matrix4x4::Identity; // Default transform
		Matrix4x4 offsetMatrix = Matrix4x4::Identity;

		unsigned int parentIndex = 0;
		unsigned int depth = 1; // Depth of node (root is depth 1)

		std::vector<unsigned int> subMeshes;
		std::vector<unsigned int> children;
	};

	// A mesh contains mutiple submeshes with each submesh using a different material
	// Each submesh is controlled by a single node in the mesh's hierachy - seen node structure above
	// Each submesh has a sperate vertex / index buffer on the GPU (could shader buffers for performance but that'd be more complex)
	struct SubMesh
	{
		unsigned int nodeIndex;
		std::string name;
		std::string materialName;

		GeometryTypes geometryTypes = GeometryTypes::Position;

		// GPU settings for material used by this submesh (shaders, constant buffers, texture objects etc.)
		std::unique_ptr<RenderState> renderState;

		unsigned int polygonCount = 0;
		unsigned int* indices;
		std::vector<Vector3> polygonVertices;
		std::vector<Vector3> faces;

		// GPU specification of data held in a single vetex
		unsigned int vertexSize = 0; // in bytes
		CComPtr<ID3D11InputLayout> vertexLayout;

		// GPU-side vertex and index buffers
		unsigned int numVertices = 0;
		CComPtr<ID3D11Buffer> vertexBuffer;

		unsigned int numIndices = 0;
		CComPtr<ID3D11Buffer> indexBuffer;
	};

private:
	//============================
	// Private Helper Functions
	//============================

	// Read assimp node and its children and place in mNodes vector at position nodeIndex. Optionally filler out nodes with no submeshes (filterEmpty)
	// Recusrive function, first call only needs first two parameters. Returns first nodeIndex into mNodes after the nodes inserted
	unsigned int ReadNodes(aiNode* assimpNode, bool filterEmpty, unsigned int nodeIndex = 0, unsigned int parentIndex = 0,
		unsigned int depth = 1, Matrix4x4 filteredTransform = Matrix4x4::Identity);

	// Helper function for Render function - render a given sub-mesh. World matrices / textures / states etc. must already be set
	void RenderSubMesh(const SubMesh& subMesh);

private:
	//===============
	// Private Data
	//===============

	std::vector<Node> mNodes; // The mesh hierarchy. First entry is root. Reminder are stored in depth-first order
	std::vector<SubMesh> mSubMeshes; // The mesh geometry. Nodes refer to sub-mesh in this vector

	// Each node above has a transform relative in its parent. Each render we mutiple these out to get every transform in world space
	std::vector<Matrix4x4> mAbsoluteTransforms;

	unsigned int mMaxNodeDepth = 0; // Depth of deepest node in hierarchy (root is depth 1)

	bool mHasBones; // if submesh has bones, then all submeshes are given modes - makes rendering easeir (one shader for the whole mesh)

	std::filesystem::path mFilePath; // Full path name to mesh source file, or empty path if mesh did not originate from a file
};

#endif // !_MESH_H_INCLUDED_
