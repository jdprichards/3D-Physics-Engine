//===========================================
// Entity and EntityTemplate Base Classes
//===========================================
// An entity template is a blueprint for a type of entity. We can "instantiate" entites into the scene
// using this template data. The templates contrain the geometry used by the entity type as well as any 
// shadered data that all entites of that type will use (e.g. Max HP, Top Speed, etc.)
//
// An entity is a single instance of an entity template in the scene, holding positional information and
// any data specific to taht instance (e.g. Current HP, Current Speed, etc.)
//
// The base EntityTemplate class only contains geometry with no other data. This is sufficient for static
// entity types or those that don't need any data at the template level. Inherited template classes are used
// for more complex types of entity that need more shared data
//
// The base Entity class can handle the rendering needs of most entity types. However, there is no entity data
// except the trasformation matrices, and the Update functions does the bare minimum (simple death check). So
// this class is only suitable for static scene objects. More complex entities wil have other gameplay data
// and code so they will need to have new classes inherited from the base Entity
//
// An EntityTemplate can return the collection of entities currently using it so the game code may not need
// to maintain the same data. E.g. if you need to do something will all of a certain type of entities
// you could use that entities template's list of the current instances of that entity rather than hold your own.
// However, the template holds pointer to the entity base class and you may need to cast to the actual type.

#ifndef _ENTITY_H_INCLUDED_
#define _ENTITY_H_INCLUDED_

#include "EntityTypes.h"

#include "Mesh.h"
#include "Matrix4x4.h"
#include "Matrix3x3.h"
#include "Vector3.h"
#include "ColourTypes.h"

#include "BoundingVolume.h"

#include <vector>
#include <string>
#include <memory>

// Forward declration of Mesh class allows us to use Mesh pointer before that class has been fully declared
// This reduces the number of include files needed here, which in turn minimises dependencies and speeds up compilation
//
// Note: if you have a class contraing a unique_ptr to a forward declaration (that is the case here) then this class must also have a destructor
// signatrue here and declaration in the cpp file - this is required even if the destructor is default / empty

class Mesh;
class BoundingVolume;

enum class ECollisionType;


//==========================
// Entity Template Class
//==========================
class EntityTemplate
{
	friend class EntityManager; // Manager is a friend class so it can update which entities use this template without needing o expose a public function

public:
	//=============================
	// Construction / Destruction
	//=============================

	// Base entity template constructor needed template name (e.g. sky) and the associated mesh (e.g. sky.fbx)
	// Can throw std::runtime_error if he mesh fails to load
	EntityTemplate(const std::string& type, const std::string& meshFilename, ImportFlags importFlags = {});

	// Destructor - polymorphic base class destructors should always be virtual
	virtual ~EntityTemplate();

public:
	//=================
	// Data Access
	//=================

	// The name of the type of entity this template is a blueprint for
	std::string& GetType()
	{
		return mType;
	}

	// Returns reference to the mesh used by entities based on this template
	Mesh& GetMesh()
	{
		return *mMesh;
	}

	// Return a vector of entity IDs using this template
	const std::vector<EntityID>& Entites()
	{
		return mEntities;
	}



private:
	//================
	// Private Data
	//================

	// Type of the template
	std::string mType;

	// Mesh representing this entity
	std::unique_ptr<Mesh> mMesh;

	// Pointers to entities based on this template
	std::vector<EntityID> mEntities;

};


class Entity
{
public:
	//================================
	// Construction and Destruction
	//================================

	// Base entity constructor, needs pointer to common template data and unique ID (UID)
	// May also pass a name and inital transformation for root (defaults are empty named entity at origin)
	Entity(EntityTemplate& entityTemplate, EntityID UID,
		const Matrix4x4& transform = Matrix4x4::Identity, const std::string& name = "");

	// Destructor - polymorphic base calss destuctors should be virtual
	virtual ~Entity();

public:
	//=================
	// Data Access
	//=================

	// Diect access to entity's template, can access base of inheritied classes
	// 
	// No template parameter:
	//	string templateType = entity->Template().GetType()
	// 
	// Template parameter:
	//  float walkSpeed = entity->Template()<Enemy>().mWalkSpeed;
	template<typename T = EntityTemplate> T& Template() { return dynamic_cast<T&>(mTemplate); }

	// Entity identity getters
	EntityID GetID() { return mID; }
	const std::string& GetName() { return mName; }

	// Direct access to transformation matrix of the given node of the entities mesh.
	// Doot matrix (node = 0) is in world space, all other nodes are parent-relative
	// 
	// Matrix4x4 supports many functions for model manipulation - see Matrix4x4.h
	// Examples: entity.Transform().Position() = {0,0,0};  entity.Transform(2).SetScale(10);  Vector3 facing = entity.Transform().ZAxis();
	//			 entity.Transform().FaceTarget(enemyPosition);  Vector3 angles = entity.Transform(1).GetRotation();
	Matrix4x4& Transform(int node = 0) { return mTransforms[node]; }
	std::vector<Matrix4x4> Transforms() { return mTransforms; }

	Vector3& Position() { return Transform().Position(); }

	// Calculate the absolute transformation matrix of a given node. All nodes except rood 0 store their transformations relative to their parent.
	// This method gives the real world-space transformation of a node
	Matrix4x4 AbsoluteTransform(int node) { return mTemplate.GetMesh().AbsoluteMatrix(mTransforms, node); }

	// Direct access to the render group value for this entity. Default is 0 but the value can be get or set
	// Each enttiy has a render group valeu and groups of entities with the same value can be rendered seperetly
	// The group values are for the call to decide
	// Example:
	//	Give index 0 to entitie rendered with no blending and 1 to entities rendered with alpha blending
	//	Then render group 0 and 1 seperately, changing render states between rather than for each entity
	unsigned int& RenderGroup() { return mRenderGroup; }

	// Direct access to the render colour of this entity. value can be get or set
	ColourRGBA& RenderColour() { return mRenderColour; }

	// Access to Entity Type
	EEntityType& EntityType() { return mEntityType; }

	ECollisionType& CollisionType();
	template<typename T> T* GetBoundingVolume();

	float& Mass() { return mMass; }
	float& InverseMass() { return mInverseMass; }
	float& Density() { return mDensity; }
	float& Volume() { return mVolume; }
	float& FrictionCoefficient() { return mFrictionCoefficient; }
	Vector3& CenterOfMass() { return mCenterOfMass; }

	Vector3& Velocity() { return mVelocity; }
	Vector3& AngularVelocity() { return mAngularVelocity; }

	Matrix3x3& InertiaTensor() { return mInertiaTensor; }
	Matrix3x3& InverseInertiaTensor() { return mInverseInertiaTensor; }


	float& CoefficientRestitution() { return mCoefficientRestitution; }
	/*void SetAABB(Vector3& minAABB, Vector3& maxAABB)
	{
		mAABBBoundingMin = minAABB;
		mAABBBoundingMax = maxAABB;
	}

	void GetAABB(Vector3& minAABB, Vector3& maxAABB)
	{
		minAABB = mAABBBoundingMin;
		maxAABB = mAABBBoundingMax;
	}

	void GetRelativeAABB(Vector3& minAABB, Vector3& maxAABB)
	{
		minAABB = mAABBBoundingMin + Position();
		maxAABB = mAABBBoundingMax + Position();
	}

	void SetSphereBB(Vector3& center, float& radius)
	{
		mBoundingSphereCenter = center;
		mBoundingSphereRadius = radius;
	}

	void GetSphereBB(Vector3& center, float& radius)
	{
		center = mBoundingSphereCenter;
		radius = mBoundingSphereRadius;
	}

	void GetRelativeSphereBB(Vector3& center, float& radius)
	{
		center = mBoundingSphereCenter + Position();
		radius = mBoundingSphereRadius;
	}

	Vector3& AABBoundingBoxMin() { return mAABBBoundingMin; }
	Vector3& AABBoundingBoxMax() { return mAABBBoundingMax; }

	Vector3& SphereBoundingCenter() { return mBoundingSphereCenter; }
	float& SphereBoundingRadius() { return mBoundingSphereRadius; }*/

	void SetLifeTime(float lifeTime);
	void RemoveLifetime();

public:
	//===================
	// Update / Render
	//===================

	// Update the entity including message processing for each entity
	// Virtual function, inherited classes shoiuld override this with their own processing
	//	Entity Update functions should return false if entity is to be destroyed
	virtual bool Update(float frameTime);

	// Render the entity's geometry
	void Render();

	// Name for the entity, can be empty "" doesn't need to be unique
	std::string mName;

protected:
	float mMass = 0;
	float mInverseMass = 0;

	float mDensity = 100;

	float mVolume = 0;
	Vector3 mCenterOfMass = { 0,0,0 };

	float mFrictionCoefficient = 0.25f;

	float mCoefficientRestitution;
	//Vector3 mBoundingSphereCenter;
	//float mBoundingSphereRadius;

	//Vector3 mAABBBoundingMin;
	//Vector3 mAABBBoundingMax;

	Matrix3x3 mInertiaTensor;
	Matrix3x3 mInverseInertiaTensor;

	Vector3 mVelocity = { 0,0,0 };
	Vector3 mAngularVelocity = { 0,0,0 };

	EEntityType mEntityType;

	std::unique_ptr<BoundingVolume> mBoundingVolume;

	bool mHasLifeTime = false;
	float mLifeTime = 0.0f;
	float mTimeElapsed = 0.0f;

private:
	float CalculateInertiaMomentX(std::vector<Vector3> vertex); // Axis to caclulate moment of inertia along
	float CalculateInertiaMomentY(std::vector<Vector3> vertex);

	float CalculateInertiaMomentZ(std::vector<Vector3> vertex);
	float CalculateInertiaProductYZ(std::vector<Vector3> vertex); // Axes to calculate moment of inertia along (order matters)
	float CalculateInertiaProductXZ(std::vector<Vector3> vertex);
	float CalculateInertiaProductXY(std::vector<Vector3> vertex);

	void CalculateInertiaTensor();
	void CalculateMass();
	void CalculateVolume();
private:
	//================
	// Private Data
	//================

	// Template used by this entity
	EntityTemplate& mTemplate;

	// Unique identifies for the entity
	EntityID mID;

	// Transformation matrices that position this entity. mTransform[0] is the world matrix for the entire model. Remaining transforms position sub-parts of
	// the model with each matrix reation to parent part. Hierarchy tree defined in the entity template -> mesh
	std::vector<Matrix4x4> mTransforms;

	// Each entity has a render group value and the groups of entites with the same vallue canm be rendered seperately
	unsigned int mRenderGroup;

	// Each entity can have a custom colour
	ColourRGBA mRenderColour = { 1, 1, 1, 1 };
};

#endif // !_ENTITY_H_INCLUDED_
