#ifndef _LIGHT_H_DEFINED_
#define _LIGHT_H_DEFINED_

#include "Entity.h"
#include "SceneGlobals.h" // For entity manager and messegner
#include "Scene.h"
#include "Mesh.h"

class Entity;
class EntityManager;

class Light : public Entity
{
public:
	Light(EntityTemplate& entityTemplate, EntityID ID,
		const Matrix4x4& transform = Matrix4x4::Identity, ColourRGBA colour = ColourRGBA(1.0f, 1.0f, 1.0f, 1.0f), const std::string name = "");

public:
	ColourRGBA mColour;
	float mIntensity;
};

#endif // _LIGHT_H_DEFINED_
