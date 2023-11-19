#include "Events.h"

void Events::OnEntityTookDamage(Entity* victim, Entity* attacker, int damage)
{

}

void Events::OnPlayerDeath(Player* victim, Entity* attacker)
{
	if (Game::IsValidEntity(attacker))
	{
		if (attacker->IsPlayer())
			std::cout << "Player " << victim->ID() << " was killed by Player " << attacker->ID() << "!\n";
		else if (attacker->IsNPC())
			std::cout << "Player " << victim->ID() << " was killed by NPC " << attacker->ID() << "!\n";
		else
			std::cout << "Player " << victim->ID() << " was killed by Entity " << attacker->ID() << "!\n";
	}
	else
	{
		std::cout << "Player " << victim->ID() << " died!\n";
	}
}

void Events::OnNPCDeath(NPC* victim, Entity* attacker)
{
	if (Game::IsValidEntity(attacker))
	{
		if (attacker->IsPlayer())
			std::cout << "NPC " << victim->ID() << " was killed by Player " << attacker->ID() << "!\n";
		else if (attacker->IsNPC())
			std::cout << "NPC " << victim->ID() << " was killed by NPC " << attacker->ID() << "!\n";
		else
			std::cout << "NPC " << victim->ID() << " was killed by Entity " << attacker->ID() << "!\n";
	}
	else
	{
		std::cout << "NPC " << victim->ID() << " died!\n";
	}
}