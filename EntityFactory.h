#pragma once

#include "Entity.h"
#include "Player.h"
#include "Vehicle.h"
#include "NPC.h"

class EntityFactory
{
public:
	static Entity* CreateEntity();
	static Player* CreatePlayer(bool isLocal = false);
	static Vehicle* CreateVehicle();
	static NPC* CreateNPC();
};