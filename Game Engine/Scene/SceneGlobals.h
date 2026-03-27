//====================================
// Scene Related Global Variables
//====================================
// This applications is mostly well-encapsulated, but there are a few globals in use to simplify the code architecture
// 
// The gEntityManager object is gobal mainly so any entity class can access to it look up other entities by ID
//
// The gMessenger object is global so enttiy clas can send messages to other entities
//
// It would be possilbe to deal with these as "singleton" glasses. These are classes that only ever have a single object.
// It might make the code a tiny bit more cleaner but the benifit is marginal - singletons have the same downsides as globals

#ifndef _SCENE_GLOBALS_H_INCLUDED_
#define _SCENE_GLOBALS_H_INCLUDED_

#include "EntityManager.h"
#include "Messenger.h"
#include "CollisionDetection.h"

#include "ColourTypes.h"

#include <memory>
#include <string>

//==================
// Globals
//==================

// Global Entity Manager
extern std::unique_ptr<EntityManager> gEntityManager;

// Global Messenger
extern std::unique_ptr<Messenger> gMessenger;

extern std::unique_ptr<CollisionDetection> gCollisionDetection;

extern ColourRGB gAmbientColour;

extern float PI;

extern std::map<EEntityType, std::string> entityTypeMap;

extern int gCurrentDemo;

extern float gFPS;

#endif // !_SCENE_GLOBALS_H_INCLUDED_
