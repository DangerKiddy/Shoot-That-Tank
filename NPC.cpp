#include "NPC.h"
#include "Math.h"
#include "Game.h"
#include "Ballistics.h"
#include "Events.h"

NPC::NPC()
{
	type = Type::NPCType;
	SetHealth(100);
}

NPC::~NPC()
{
}

void NPC::SetTeam(Player::Team team)
{
	this->team = team;
}

const Player::Team NPC::GetTeam()
{
	return team;
}

void NPC::Draw(ImDrawList* canvas)
{
	if (!IsAlive())
		return;

	auto dir = lookDirection;
	auto left = Game::Math::VecCross(dir);
	auto right = Game::Math::VecMul(left, -1);

	auto base = Game::Math::VecSub(position, Game::Math::VecMul(dir, GetSize()));
	auto p1 = Game::Math::VecAdd(base, Game::Math::VecMul(left, GetSize()));
	auto p2 = Game::Math::VecAdd(base, Game::Math::VecMul(right, GetSize()));
	auto p3 = Game::Math::VecAdd(position, Game::Math::VecMul(dir, GetSize()));

	canvas->AddTriangle(p1, p2, p3, GetColor());

	Entity::DrawHealth(canvas);
}

void NPC::Tick()
{
	Entity::Tick();

	SetupColors();

	if (!IsAlive())
	{
		if (respawnAtTime < Game::Time())
			Spawn();

		return;
	}

	AI();
}

void NPC::SetupColors()
{
	if (Game::IsValidEntity(Game::LocalPlayer))
	{
		if (team == Game::LocalPlayer->GetTeam())
			SetColor(IM_COL32(0, 0, 255, 255));
		else
			SetColor(IM_COL32(255, 0, 0, 255));
	}
	else
		SetColor(IM_COL32(0, 0, 255, 255));
}

void NPC::AI()
{
	if (!Game::IsValidEntity(targetEntity) || !CanTarget(targetEntity))
	{
		FindTarget();
	}
	else
	{
		Aim(targetEntity->GetPos());

		Shoot();
	}

	if (Game::Time() > nextChangeDirection)
	{
		ImVec2 dir{ Game::Math::RandomFloat(-1, 1), Game::Math::RandomFloat(-1, 1) };
		Game::Math::VecNormalize(dir);
		moveDirection = dir;

		nextChangeDirection = Game::Time() + Game::Math::RandomFloat(aiChangeDirectionTime / 2.f, aiChangeDirectionTime);
	}

	if (Game::Math::VecLength(moveDirection) > 0)
		SetVelocity(Game::Math::VecMul(moveDirection, GetWalkSpeed()));
}

Entity* NPC::GetTarget()
{
	return targetEntity;
}

void NPC::FindTarget()
{
	std::vector<Entity*> possibleTargets{};

	for (auto it = std::begin(Game::entities); it != std::end(Game::entities);)
	{
		auto ent = it->second;
		++it;

		if (!CanTarget(ent))
			continue;

		possibleTargets.push_back(ent);
	}

	if (possibleTargets.size() > 0)
		targetEntity = possibleTargets.at(Game::Math::RandomInt(0, possibleTargets.size() - 1));
}

bool NPC::CanTarget(Entity* ent)
{
	if (!Game::IsValidEntity(ent) || ent == this)
		return false;

	switch (ent->GetType())
	{
	case Entity::VehicleType:
	{
		auto veh = (Vehicle*)ent;
		if (!Game::IsValidEntity(veh->GetDriver()))
			return false;

		ent = (Entity*)veh->GetDriver();
		[[fallthrough]];
	}

	case Entity::NPCType:
		break;

	case Entity::PlayerType:
	{
		auto ply = (Player*)ent;
		return ply->GetTeam() != team && ply->IsAlive();
	}

	case Entity::Unknown:
	case Entity::ProjectileType:
	default:
		break;
	}

	return false;
}

void NPC::Aim(ImVec2 pos)
{
	ImVec2 dir = Game::Math::VecSub(pos, GetPos());
	Game::Math::VecNormalize(dir);

	lookDirection = dir;
}

void NPC::Shoot()
{
	if (CanShoot())
	{
		FireBullet();
		nextShoot = Game::Time() + shootSpeed;
	}
}

bool NPC::CanShoot()
{
	return Game::Time() >= nextShoot;
}

const float NPC::GetWalkSpeed()
{
	return walkSpeed;
}

bool NPC::IsAlive()
{
	return GetHealth() > 0;
}

void NPC::FireBullet()
{
	auto bullet = new Bullet();
	bullet->SetPos(GetPos());
	bullet->SetDamage(25);
	bullet->SetOwner(this);
	bullet->SetSpeed(20);
	bullet->SetDirection(lookDirection);
	bullet->SetSize(25);
}

bool NPC::ShouldCollide(Entity* otherEnt)
{
	if (otherEnt->GetType() == Entity::Type::ProjectileType && !IsAlive())
		return false;

	return true;
}

void NPC::Damage(int dmg, Entity* attacker)
{
	Entity::Damage(dmg, attacker);

	if (GetHealth() <= 0)
	{
		Kill(attacker);
	}
}

void NPC::Kill(Entity* attacker)
{
	SetHealth(0);

	respawnAtTime = Game::Time() + respawnTime;

	Events::OnNPCDeath(this, attacker);
}

void NPC::Spawn()
{
	SetPos(Game::GetRandomWorldPos());

	SetHealth(100);
}