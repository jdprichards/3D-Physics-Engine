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


#include "SceneGlobals.h"

//===========
// Globals
//===========

// Global Entity Manager
std::unique_ptr<EntityManager> gEntityManager;

// Global Messenger
std::unique_ptr<Messenger> gMessenger;

std::unique_ptr<CollisionDetection> gCollisionDetection;

ColourRGB gAmbientColour = { 0,0,0 };

float PI = 3.1415f;

std::map<EEntityType, std::string> entityTypeMap =
{
	{EEntityType::Static, "Static"},
	{EEntityType::RigidBody, "RigidBody"},
	{EEntityType::Light, "Light"}
};

int gCurrentDemo = 0;

extern float gFPS = 0;