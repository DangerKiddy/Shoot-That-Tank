#include "EntityFactory.h"

Entity* EntityFactory::CreateEntity()
{
    auto ent = new Entity();

    return ent;
}

Player* EntityFactory::CreatePlayer(bool isLocal)
{
    auto ply = new Player();

    if (isLocal)
        ply->InitLocal();
    else
        ply->Init();

    return ply;
}

Vehicle* EntityFactory::CreateVehicle()
{
    auto veh = new Vehicle();

    return veh;
}

NPC* EntityFactory::CreateNPC()
{
    auto npc = new NPC();

    return npc;
}