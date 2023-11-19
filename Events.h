#pragma once
#include "Game.h"
#include "NPC.h"

namespace Events
{
	void OnEntityTookDamage(Entity* victim, Entity* attacker, int damage);
	void OnPlayerDeath(Player* victim, Entity* attacker);
	void OnNPCDeath(NPC* victim, Entity* attacker);
}