//=================================================================================
// EntityManager class encapsulates creation, update and rendering of entities 
//=================================================================================
// The EntityManager can create and destory entities and entity templates
// Each entity template is identified by a type (string), and existing templates can be
// searched for by type. Each new entity is given a unique ID and existing entities can be
// searched for by ID. The manager can also run the Update and Render functions of all entities
// Certain rendering setting can be applied at per-entity level
// e.g transprancey, tint colour, etc.

#ifndef _ENTITY_MANAGER_H_INCLUDED_
#define _ENTITY_MANAGER_H_INCLUDED_

#include "Entity.h"
#include "RigidBody.h"
#include "Light.h"

#include <string>
#include <map>
#include <stdexcept>
#include <stdint.h>

//=========================
// Entity Manager Class
//=========================
class EntityManager
{
public:
	//===============================
	// Entity / Template Creation
	//===============================

	// Create any type of entity template that inherits from the EnttiyTemplate
	// Returns nullptr if enttiy template creation fails. Call GetLastError() for a text description of the error
	// There is no need to delete the returned pointer as this manages handles object lifetimes. 
	// Entity creation is done by template name, not pointer
	//
	// All entity templates must have a construtor whose first two parameters are type nd meshFilename. However, they may
	// also have furthure parameters. When calling this functions, after passing the type and meshFilename, also pass any other
	// constructor parameters neededby the template type you're creating. See template constructors in any model class for an example
	// It is a good practice to include importFlags parameter in new templates so the mesh import of you template can be
	// tweaked
	//
	//	E.g. EntityTemplate* sky = CreateEntityTemplate<EntityTemplate>("sky", "sky.fbx");
	//
	// The extra parameters are forwarded to the constructor using a parameter pack (...) and std::forward. A very powerful but ugly syntax
	// ConstructorTypes and constructorValues use ...
	template <typename EntityTemplateType, typename ...ConstructorTypes>
	EntityTemplateType* CreateEntityTemplate(std::string type, std::string meshFilename, ConstructorTypes&&... constructorValues)
	{
		// If template with same name already exists, if so delete existing one
		if (mEntityTemplates.contains(type))
			mEntityTemplates.erase(type);

		// Try to construct new entity template
		EntityTemplateType* entityTemplate;
		try
		{
			entityTemplate = new EntityTemplateType(type, meshFilename, std::forward<ConstructorTypes>(constructorValues)...);
		}
		catch (std::runtime_error e)
		{
			mLastError = e.what(); // Picks up error message in exception
			return nullptr;
		}

		// Add new entity template to map (use map.emplace because map contrains unique_ptr)
		mEntityTemplates.emplace(type, entityTemplate);

		return entityTemplate;
	}

	// Create any Type of Entity that inherits from Entity
	// Returns NO_ID if the template type does not exist or if entity creation fails. Call GetLastError for a text description of the error
	//
	// All entities must have a constructor whos first two parameters are an EntityTemplate& then an EntityID. However, they may
	// also have further parameters. When calling this function first pass template name (manager will create ID)
	// Then pass other parameters when the entity's constructor expects
	// Note that even the base class entity type has further constructor parameters aside from template name
	// E.g:
	// - Entity* Tree1 = CreateEntity<Entity>("Tree");
	// - Entity* Tree2 = CreateEntity<Entity>("Tree", Matrix({10, 0, 20}), "Joe");
	// - Entity* Tank1 = CreateEnttiy<Tank>("Tank" 100, Matrix4x4({10, 0, 20}), "Joe");
	//
	// Extra parameters are forwarded to constructor using a parameter pack(...) and std::forward
	template <typename EntityType, typename ...ConstructorTypes>
	EntityID CreateEntity(std::string templateType, ConstructorTypes&&... constructorValues)
	{
		// Check that requested enetiy template exists
		if (!mEntityTemplates.contains(templateType))
		{
			mLastError = "Entity Manager: Cannot find entity template '" + templateType + "'";
			return NO_ID;
		}

		// Get template and ID for new entity
		EntityTemplate* entityTemplate = mEntityTemplates[templateType].get();
		EntityID newID = mNextID++;

		EntityType* entity;
		try
		{
			entity = new EntityType(*entityTemplate, newID, std::forward<ConstructorTypes>(constructorValues)...);

		}
		catch (std::runtime_error e)
		{
			mLastError = e.what(); // gets the error message from the exception
			return NO_ID;
		}

		// Add new enttiy to map (map.emplace is needed instead of map.insert because the map contrains a unique_ptr)
		mEntities.emplace(newID, entity);

		// Template about this new entity that is using it
		entityTemplate->mEntities.push_back(newID);

		return newID;
	}

public:
	//=============================
	// Entity / Template Deletion
	//=============================

	// Destroy the given entity template, also destroying all the entities that are based on it
	// Entity templates can use a lot of memory (meshs and textures) so they should be released when possible 
	// Returns true on success
	// Returns false if there isn't an entity template with this given type
	bool DestoryEntityTemplate(std::string type);

	// Destroy the entity with the given ID
	// Returns true on sucess
	// Returns false if there isn't an entity with this ID
	bool DestroyEntity(EntityID id);

public:
	//===========
	// Access
	//===========

	// Returns the entity template with the given type name. Returns as a base calss EntityTemplate pointer by default but if you
	// know the inherited type you can specify it to get a pointer to the inherited type
	// E.g.
	// - EntityTemplate* baseTemplate = myEntityManager->GetTemplate("Basic Wizard")
	// - WizardTemplate* wizardTemplate = myEntityManager->GetTemplate<WizardTemplate>("Basic Wizard")
	// Returns nullptr if no template of the given name is found or if invalid template type is used
	template<typename T = EntityTemplate>
	T* GetTemplate(std::string type)
	{
		if (!mEntityTemplates.contains(type))
			return nullptr;

		T* entityTemplate;

		try
		{
			entityTemplate = dynamic_cast<T*>(mEntityTemplates[type].get());
		}
		
		catch (std::bad_cast e)
		{
			return nullptr;
		}

		return entityTemplate;
	}

	// Returns the entity with the given name. Returns as a base class Entity pointer by default, can specify the inherited entity type
	// E.g
	// - Entity* tankBase = myEntityManager->GetEntity("myTank");
	// - Tank* tank = myEntityManager0>GetEntity<Tank>("myTank");
	// Returns nullptr if no entity with given name exists or invalid entity type

	template <typename T = Entity>
	T* GetEntity(std::string name)
	{
		// Search for the name
		for (auto& entity : mEntities)
			if (entity.second->GetName() == name)
				return dynamic_cast<T*>(entity.second.get());

		return nullptr;
	}

	// Returns the entity with the given ID. Returns as a base class Entity pointer by default, can specify the inherited entity type
	// E.g
	// - Entity* tankBase = myEntityManager->GetEntity(tankID);
	// - Tank* tank = myEntityManager0>GetEntity<Tank>(tankID);
	// Returns nullptr if no entity with given ID exists or invalid entity type
	template <typename T = Entity>
	T* GetEntity(EntityID id)
	{
		if (!mEntities.contains(id))
			return nullptr;

		T* entity;
		try
		{
			entity = dynamic_cast<T*>(mEntities[id].get());
		}
		catch (std::bad_cast e)
		{
			return nullptr;
		}

		return entity;
	}

	std::map<EntityID, std::unique_ptr<Entity>>& GetEntities()
	{
		return mEntities;
	}

public:
	//===================
	// Entity Ussage
	//===================

	// Render all entities in a particular render group (render group comments in Entity.h)
	void RenderGroup(unsigned int group);

	// render all entities regardless of group
	void RenderAll();

	// Call all current entity's Update functions. Any entity that returns false will be destroyed
	void UpdateAll(float frameTime);

	// If the CreateEntityTemplate or CreateEntity functons reutnr nullptr to indicate an error
	// the text description of the most recent error can be fetched
	std::string GetLastError()
	{
		return mLastError;
	}

	// Clears any error
	void ClearLastError()
	{ 
		mLastError = "";
	}

private:
	//=================
	// Private Data
	//=================

	// Entity templates are ordered and searched for by name
	std::map<std::string, std::unique_ptr<EntityTemplate>> mEntityTemplates;

	// Entities are ordered and seached for by ID (better performance than searching by name)
	std::map<EntityID, std::unique_ptr<Entity>> mEntities;

	// The next entity will get this ID, increment for each creation
	EntityID mNextID = FIRST_ENTITY_ID;

	// Description of the most recent error from CreateEntityTemplate or CreateEntity
	std::string mLastError;
};

#endif // !_ENTITY_MANAGER_H_INCLUDED_
