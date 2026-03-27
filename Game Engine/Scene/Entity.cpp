//=================================================
// Entity class, base class for all scene elements
//=================================================
// The entity base call holds a pointer to its template class, ID, and name and current transformations of its geometry
//
// This base Entity calss can handle the render needs of most entity types. However there is no entity data here
// except for the transformation matrices, and the update function does the bare minium (death checks)
// So class is only suitable for static scene objects. More complex entities will have other gameplay data and code in inherited classes

#include "Entity.h"
#include "Mesh.h"
#include "SceneGlobals.h" // For entity manager and messegner

#include "BoundingVolume.h"
#include "BoundingSphere.h"
#include "AABB.h"
#include "OBB.h"
#include "Plane.h"

//=========================================
// Entity Template and Entity Construction
//=========================================

// Base entity template constructor needs template type and the associated mesh
// Can throw std::runtime_error if mesh fails to load
EntityTemplate::EntityTemplate(const std::string& type, const std::string& meshFilename, ImportFlags importFlags)
	: mType(type)
{
	mMesh = std::make_unique<Mesh>(meshFilename, importFlags);
}

// Destructor - Nothing to do, only required beause polymorphic base classes also have one
EntityTemplate::~EntityTemplate() {};


// Entity Constructor, needs pointer to common template data and ID
// May also pass a name and inital transformation for root 
Entity::Entity(EntityTemplate& entityTemplate, EntityID ID, const Matrix4x4& transform, const std::string& name)
	: mTemplate(entityTemplate), mID(ID), mTransforms(mTemplate.GetMesh().NodeCount()), mName(name)
{
	// Set name to model
	int nameInt = 0;

	std::string tempName = name;

	if (name == "")
		tempName = "object";

	for (auto& [id, entity] : gEntityManager->GetEntities())
	{
		if (entity->mName == tempName)
		{
			nameInt += 1;
		}

		if (name == "")
			tempName = "object";
		else
			tempName = name;

		if (nameInt > 0)
			tempName = tempName + std::to_string(nameInt);
	}

	mName = tempName;

	// Set inital matrices from mesh defualts

	for (int i = 0; i < mTransforms.size(); i++)
		mTransforms[i] = mTemplate.GetMesh().DefaultTrasformation(i);

	// Override default root matrix with constuctor parameters
	mTransforms[0] = transform;

	mEntityType = EEntityType::Static;

	if (Template().GetType() != "Sky" && Template().GetType() != "Floor" && Template().GetType() != "Light")
	{
		CalculateInertiaTensor();

		mInverseInertiaTensor = InverseAffine(mInertiaTensor);

		mBoundingVolume = std::make_unique<AABB>(AABB(this));
	}

	if (Template().GetType() == "Wall")
	{
		mBoundingVolume = std::make_unique<AABB>((this));
		mCoefficientRestitution = 0.75f;
	}

	if (Template().GetType() == "Floor")
	{
		CalculateMass();
		mBoundingVolume = std::make_unique<Plane>(Plane(this));
		mCoefficientRestitution = 0.8f;
		mFrictionCoefficient = 1.0f;
	}

	if (Template().GetType() == "Sphere")
	{
		mBoundingVolume = std::make_unique<BoundingSphere>(BoundingSphere(this));
		mCoefficientRestitution = 0.70f;
	}
	if (Template().GetType() == "Cube")
	{
		mCoefficientRestitution = 0.25f;
	}

	if (Template().GetType() != "Floor")
	{
		mFrictionCoefficient = 0.75f;
		//mFrictionCoefficient = Random(0.25f, 1.0f);
	}

	if (mBoundingVolume != nullptr)
		mBoundingVolume->UpdateBoundingVolume();

	//if (CollisionType() == ECollisionType::BoundingSphere)
	//	CreateSphereBB(this);


	//if (CollisionType() == ECollisionType::AABB)
	//	CreateAABB(this);


	//if (CollisionType() == ECollisionType::OBB)
	//	CreateOBB(this);


}

// Destructor - Nothing to do, only required beause polymorphic base classes also have one
Entity::~Entity() {}

ECollisionType& Entity::CollisionType()
{
	if (mBoundingVolume != nullptr)
		return mBoundingVolume->GetCollisionType();
}

template<typename T> T* Entity::GetBoundingVolume()
{
	return dynamic_cast<T*>(mBoundingVolume.get());
}


//===============================
// Calculate Physics Properties
//===============================


void Entity::CalculateMass()
{
	unsigned int polygonCount = Template().GetMesh().PolygonCount();

	float mass = 0.0f;

	Vector3 centerOfMass = { 0, 0, 0 };
	Vector3 centerMass = { 0,0,0 };

	std::vector<Vector3> polygonVectices;
	std::vector<Vector3> vertices = Template().GetMesh().ScaledVertices(Transforms());
	std::vector<Vector3> tempVertices = vertices;
	int polygonsDone = 0;

	for (unsigned int i = 0; i < polygonCount * 3; i += 3)
	{
		polygonVectices = std::vector({ tempVertices[i], tempVertices[i + 1], tempVertices[i + 2] });

		float detJ = Dot(polygonVectices[0], Cross(polygonVectices[1], polygonVectices[2]));

		float polygonVolume = detJ / 6.0f;
		float polygonMass = mDensity * polygonVolume;

		Vector3 polyCenterOfMass = (polygonVectices[0] + polygonVectices[1] + polygonVectices[2]) / 4.0f;

		centerMass += polyCenterOfMass;
		centerOfMass += polyCenterOfMass * polygonMass;
		mass += polygonMass;

		polygonsDone++;
	}

	if (!IsZero(mass))
		centerOfMass /= mass;

	mCenterOfMass = centerOfMass;
	mMass = mass;
	mVolume = mMass * mDensity;
	mInverseMass = 1 / mass;
}

void Entity::CalculateVolume()
{
	unsigned int polygonCount = Template().GetMesh().PolygonCount();
	float volume = 0;

	std::vector<Vector3> vertices = Template().GetMesh().ScaledVertices(Transforms());

	for (unsigned int i = 0; i < polygonCount; i += 3)
	{
		float detJ = Dot(vertices[i], Cross(vertices[i + 1], vertices[i + 2]));
		volume += 6 * detJ;
	}

	mVolume = volume;
}

void Entity::SetLifeTime(float lifeTime)
{
	mHasLifeTime = true;
	mLifeTime = lifeTime;
	mTimeElapsed = 0.0f;
}

void Entity::RemoveLifetime()
{
	mHasLifeTime = false;
	mLifeTime = 0.0f;
	mTimeElapsed = 0.0f;
}

float Entity::CalculateInertiaMomentX(std::vector<Vector3> polygon)
{
	float result =
		Square(polygon[0].x) + polygon[1].x * polygon[2].x +
		Square(polygon[1].x) + polygon[0].x * polygon[2].x +
		Square(polygon[2].x) + polygon[0].x * polygon[1].x;

	return result;
}

float Entity::CalculateInertiaMomentY(std::vector<Vector3> polygon)
{
	float result =
		Square(polygon[0].y) + polygon[1].y * polygon[2].y +
		Square(polygon[1].y) + polygon[0].y * polygon[2].y +
		Square(polygon[2].y) + polygon[0].y * polygon[1].y;

	return result;
}

float Entity::CalculateInertiaMomentZ(std::vector<Vector3> polygon)
{
	float result =
		Square(polygon[0].z) + polygon[1].z * polygon[2].z +
		Square(polygon[1].z) + polygon[0].z * polygon[2].z +
		Square(polygon[2].z) + polygon[0].z * polygon[1].z;

	return result;
}

float Entity::CalculateInertiaProductXY(std::vector<Vector3> polygon)
{
	float result =
		2 * polygon[0].x * polygon[0].y + polygon[1].x * polygon[2].y + polygon[2].x * polygon[1].y +
		2 * polygon[1].x * polygon[1].y + polygon[0].x * polygon[2].y + polygon[2].x * polygon[0].y +
		2 * polygon[2].x * polygon[2].y + polygon[0].x * polygon[1].y + polygon[1].x * polygon[0].y;

	return result;
}

float Entity::CalculateInertiaProductXZ(std::vector<Vector3> polygon)
{
	float result =
		2 * polygon[0].x * polygon[0].z + polygon[1].x * polygon[2].z + polygon[2].x * polygon[1].z +
		2 * polygon[1].x * polygon[1].z + polygon[0].x * polygon[2].z + polygon[2].x * polygon[0].z +
		2 * polygon[2].x * polygon[2].z + polygon[0].x * polygon[1].z + polygon[1].x * polygon[0].z;

	return result;
}

float Entity::CalculateInertiaProductYZ(std::vector<Vector3> polygon)
{
	float result =
		2 * polygon[0].y * polygon[0].z + polygon[1].y * polygon[2].z + polygon[2].y * polygon[1].z +
		2 * polygon[1].y * polygon[1].z + polygon[0].y * polygon[2].z + polygon[2].y * polygon[0].z +
		2 * polygon[2].y * polygon[2].z + polygon[0].y * polygon[1].z + polygon[1].y * polygon[0].z;

	return result;
}


void Entity::CalculateInertiaTensor()
{
	unsigned int polygonCount = Template().GetMesh().PolygonCount();

	float Ix = 0.0f;
	float Iy = 0.0f;
	float Iz = 0.0f;

	float Ixy = 0.0f;
	float Ixz = 0.0f;
	float Iyz = 0.0f;

	float mass = 0.0f;

	Vector3 centerOfMass = { 0, 0, 0 };

	std::vector<Vector3> polygonVectices;
	std::vector<Vector3> vertices = Template().GetMesh().ScaledVertices(Transforms());

	// For each of the polygons within the mesh. Each polygon consists of 3 vertices
	for (unsigned i = 0; i < polygonCount * 3; i += 3)
	{
		polygonVectices = std::vector({ vertices[i], vertices[i + 1], vertices[i + 2] });

		// Calculate the volume of the polyhedron using polygon vertices. The 4th point of the polyhedron is the origin (0, 0, 0).
		float detJ = Dot(polygonVectices[0], Cross(polygonVectices[1], polygonVectices[2]));

		float polygonVolume = detJ / 6.0f;
		float polygonMass = mDensity * polygonVolume;

		// Calculate the center of mass of the polyhedron using the center of the mesh (0, 0, 0) as the 4th point
		Vector3 polyCenterOfMass = (polygonVectices[0] + polygonVectices[1] + polygonVectices[2]) / 4.0f;

		// Calculate Inertia compoents of individual polyhedrons, then add them to the overall models inertia components
		Ix += detJ * (CalculateInertiaMomentY(polygonVectices) + CalculateInertiaMomentZ(polygonVectices));
		Iy += detJ * (CalculateInertiaMomentX(polygonVectices) + CalculateInertiaMomentZ(polygonVectices));
		Iz += detJ * (CalculateInertiaMomentX(polygonVectices) + CalculateInertiaMomentY(polygonVectices));

		Ixy += detJ * (CalculateInertiaProductXY(polygonVectices));
		Ixz += detJ * (CalculateInertiaProductXZ(polygonVectices));
		Iyz += detJ * (CalculateInertiaProductYZ(polygonVectices));

		centerOfMass += polyCenterOfMass * polygonMass;
		mass += polygonMass;
	}

	// Calculate the inertia components in proportion to the density of the object and where the objects center of mass i
	centerOfMass /= mass;
	Ix *= mDensity / 60.0f - mass * (Square(centerOfMass.y) + Square(centerOfMass.z));
	Iy *= mDensity / 60.0f - mass * (Square(centerOfMass.x) + Square(centerOfMass.z));
	Iz *= mDensity / 60.0f - mass * (Square(centerOfMass.x) + Square(centerOfMass.y));

	Ixy *= mDensity / 120.0f - mass * (centerOfMass.y * centerOfMass.z);
	Ixz *= mDensity / 120.0f - mass * (centerOfMass.x * centerOfMass.y);
	Iyz *= mDensity / 120.0f - mass * (centerOfMass.x * centerOfMass.z);

	// Construct the inertia tensor using inertia compoonents
	Matrix3x3 inertiaTensor =
	{
		Ix, -Ixy, -Ixz,
		-Ixy, Iy, -Iyz,
		-Ixz, -Iyz, Iz
	};

	// Sets the various internal properties of the object
	mCenterOfMass = centerOfMass;
	mMass = mass;
	mVolume = mMass / mDensity;
	mInverseMass = 1 / mass;
	mInertiaTensor = inertiaTensor;
}

//=============================
// Entity Update / Rendering
//=============================
// 
// Update the entity including message processing for each entity
// ** Entity Update functions should return false if entity is to be destroyed **
bool Entity::Update([[maybe_unused]] float frameTime)
{
	// Add common procssing that applies to *all* entities
	//if (CollisionType() == ECollisionType::AABB)
	//	CreateAABB(this);

	//if (CollisionType() == ECollisionType::OBB)
	//	CreateOBB(this);

	//if (CollisionType() == ECollisionType::BoundingSphere)
	//	CreateSphereBB(this);

	if (mBoundingVolume != nullptr)
		mBoundingVolume->UpdateBoundingVolume();

	if (mHasLifeTime)
	{
		if (mTimeElapsed > mLifeTime)
			return false;
		else
			mTimeElapsed += frameTime;
	}

	return true;
}

// Render entity's geometry
void Entity::Render()
{
	mTemplate.GetMesh().Render(mTransforms, mRenderColour);
}

template BoundingVolume* Entity::GetBoundingVolume();
template BoundingSphere* Entity::GetBoundingVolume();
template AABB* Entity::GetBoundingVolume();
template OBB* Entity::GetBoundingVolume();
template Plane* Entity::GetBoundingVolume();
