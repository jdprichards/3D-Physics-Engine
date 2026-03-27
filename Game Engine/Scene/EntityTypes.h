//========================
// Entity-Related Types
//========================

#ifndef _ENTITY_TYPES_H_INCLUDED_
#define _ENTITY_TYPES_H_INCLUDED_

#include <stdint.h>
#include <string>
#include <map>

//=================
// Definitions
//=================

// Entity IDs are simply unsigned integers, could use uint64_t if long running app
using EntityID = uint32_t;

// Allow for some spcial IDs
const EntityID NO_ID = 0; // Special ID to inidate a null ID (e.g. return value in case of an error)
const EntityID SYSTEM_ID = 1; // SPecial ID of the system itself, can be used as the *from* parameter of messages
const EntityID FIRST_ENTITY_ID = 2;

enum class EEntityType
{
	Static,
	RigidBody,
	Light
};

#endif //_ENTITY_TYPES_H_INCLUDED_