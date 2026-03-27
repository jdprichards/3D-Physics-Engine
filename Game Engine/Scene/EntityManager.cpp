//=================================================================================
// EntityManager class encapsulates creation, update and rendering of entities 
//=================================================================================
// The EntityManager can create and destory entities and entity templates
// Each entity template is identified by a type (string), and existing templates can be
// searched for by type. Each new entity is given a unique ID and existing entities can be
// searched for by ID. The manager can also run the Update and Render functions of all entities
// Certain rendering setting can be applied at per-entity level
// e.g transprancey, tint colour, etc.

#include "EntityManager.h"

//===============================
// Entity / Template Destruction
//===============================

// Destroy the given entity template, also destroying all the entities that are based on it
// Entity templates can use a lot of memory (meshs and textures) so they should be released when possible 
// Returns true on success
// Returns false if there isn't an entity template with this given type
bool EntityManager::DestoryEntityTemplate(std::string type)
{
	// Check that requested entity template exists
	if (!mEntityTemplates.contains(type))
		return false;

	// Destroy all entities refering to this template
	auto entityTemplate = mEntityTemplates[type].get();
	while (entityTemplate->mEntities.size() > 0) // Can't use for loop as container that is being looped through is being destroyed
	{
		DestroyEntity(entityTemplate->mEntities.back());
		entityTemplate->mEntities.pop_back();
	}

	// Destroy the template
	mEntityTemplates.erase(type);
	return true;
}

// Destroy the entity with the given ID
// Returns true on sucess
// Returns false if there isn't an entity with this ID
bool EntityManager::DestroyEntity(EntityID id)
{
	// Check that requested entity exists
	if (!mEntities.contains(id))
		return false;

	// Remove entity from template collection of entities
	auto& templateEntities = GetEntity(id)->Template().mEntities;
	auto removeEnd = std::remove(templateEntities.begin(), templateEntities.end(), id);
	templateEntities.erase(removeEnd, templateEntities.end());

	mEntities.erase(id);
	return true;
}

//===============
// Entity Usage
//===============

// Render all entities in a particular render group
void EntityManager::RenderGroup(unsigned int group)
{
	for (auto& [ID, entity] : mEntities)
	{
		if (entity->RenderGroup() == group)
			entity->Render();
	}
}

// Render all entities regradless of group
void EntityManager::RenderAll()
{
	for (auto& [ID, entity] : mEntities)
		entity->Render();
}

// Call all current entity's Update functions. Any entity that returns false will be destroyed
void EntityManager::UpdateAll(float frameTime)
{
	// Loop needs to be preformed carefully since entities can be erased
	for (auto it = mEntities.begin(); it != mEntities.end();)
	{
		auto entity = it->second.get();
		it++; // Do this before ponetial entity destruction on next line

		// If entity update returns false, the entity is destroyed
		if (!entity->Update(frameTime))
			DestroyEntity(entity->GetID());
	}
}