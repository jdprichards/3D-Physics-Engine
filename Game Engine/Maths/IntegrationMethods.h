#ifndef _INTEGRATION_METHODS_H_DEFINED_
#define _INTEGRATION_METHODS_H_DEFINED_

#include "RigidBody.h"
#include "Entity.h"

void EulersMethod(Entity* entity, float frameTime);
void RungeKuttaMethod(Entity* entity, float frameTime);

#endif // _INTEGRATION_METHODS_H_DEFINED_
