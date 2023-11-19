#include "Player.h"
#include "Game.h"
#include "Math.h"
#include "Ballistics.h"
#include "Events.h"

#include <string>
#include <vector>

Player::Team Player::GetOppositeTeam(Team team)
{
	switch (team)
	{
	case Player::None:
		return Player::Red;

	case Player::Red:
		return Player::Blue;

	case Player::Blue:
		return Player::Red;

	default:
		break;
	}

	return Player::None;
}

Player::Player()
{
	team = Team::None;
	type = Type::PlayerType;
}

Player::~Player()
{
}

void Player::Init()
{
	Entity::Init();

	SetHealth(100);
	SetStiffness(.5f);
}

void Player::InitLocal()
{
	isLocal = true;
	Init();
}

const bool Player::IsLocal()
{
	return isLocal;
}

void Player::Spawn()
{
	SetPos(Game::GetWorldCenter());

	SetHealth(100);
}

void Player::SetupColors()
{
	if (!isLocal && Game::IsValidEntity(Game::LocalPlayer))
	{
		if (team == Game::LocalPlayer->team)
			SetColor(IM_COL32(0, 0, 255, 255));
		else
			SetColor(IM_COL32(255, 0, 0, 255));
	}
	else
		SetColor(IM_COL32(0, 0, 255, 255));
}

void Player::Kill(Entity* attacker)
{
	SetHealth(0);

	respawnAtTime = Game::Time() + respawnTime;

	Events::OnPlayerDeath(this, attacker);
}

void Player::Draw(ImDrawList* canvas)
{
	if (!IsAlive())
		return;

	if (!IsInVehicle())
		Entity::Draw(canvas);

	if (isLocal)
		canvas->AddText(Game::Math::VecAdd(position, ImVec2{-10, 15.f}), IM_COL32(255, 255, 255, 255), "You");
	else
	{
		std::string text = "Player " + std::to_string(ID());

		canvas->AddText(Game::Math::VecAdd(position, ImVec2{ -10, 15.f }), IM_COL32(255, 255, 255, 255), text.c_str());
	}

	if (IsInVehicle())
		return;

	canvas->AddLine(position, Game::Math::VecAdd(position, Game::Math::VecMul(GetDirection(), GetSize())), GetColor());

	Entity::DrawHealth(canvas);
}

const float Player::GetWalkSpeed()
{
	return walkSpeed;
}

const ImVec2 Player::GetMoveDirection()
{
	return moveDirection;
}

const ImVec2 Player::GetDirection()
{
	return lookDirection;
}

void Player::SetDirection(ImVec2 dir)
{
	Game::Math::VecNormalize(dir);

	lookDirection = dir;
}

void Player::Tick()
{
	Entity::Tick();

	SetupColors();

	if (!IsAlive())
	{
		if (respawnAtTime < Game::Time())
			Spawn();

		return;
	}

	if (IsInVehicle())
		SetPos(vehicle->GetPos());

	if (IsLocal())
	{
		Control();

		ImGuiIO& io = ImGui::GetIO();
		ImVec2 cursor = io.MousePos;
		Aim(cursor);

		if (ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0) && !Game::isInteractiveHovered)
		{
			if (IsInVehicle())
				vehicle->Shoot();
			else
				Shoot();
		}
	}
	else
	{
		AI();
	}
}

void Player::Control()
{
	ImVec2 dir{};
	if (Game::IsKeyDown('W'))
	{
		dir = Game::Math::VecAdd(dir, ImVec2{ 0, -1 });
	}
	else if (Game::IsKeyDown('S'))
	{
		dir = Game::Math::VecAdd(dir, ImVec2{ 0, 1 });
	}

	if (Game::IsKeyDown('A'))
	{
		dir = Game::Math::VecAdd(dir, ImVec2{ -1, 0 });
	}
	else if (Game::IsKeyDown('D'))
	{
		dir = Game::Math::VecAdd(dir, ImVec2{ 1, 0 });
	}

	Game::Math::VecNormalize(dir);

	if (IsInVehicle())
	{
		moveDirection = ImVec2{ 0,0 };

		if (Game::Math::VecLength(dir) > 0)
		{
			vehicle->SetThrottle(Game::Math::Lerp(.25f, vehicle->GetThrottle(), 1));
			vehicle->Drive(dir);
		}
		else
			vehicle->SetThrottle(0);
	}
	else
	{
		if (Game::Math::VecLength(dir) > 0)
			SetVelocity(Game::Math::VecMul(dir, GetWalkSpeed()));

		moveDirection = dir;
	}
}

void Player::Aim(ImVec2 pos)
{
	ImVec2 dir = Game::Math::VecSub(pos, GetPos());
	Game::Math::VecNormalize(dir);

	SetDirection(dir);

	if (IsInVehicle())
		vehicle->SetTargetAimPos(pos);
}

void Player::Shoot()
{
	if (CanShoot())
	{
		FireBullet();
		nextShoot = Game::Time() + shootSpeed;
	}
}

bool Player::CanShoot()
{
	return Game::Time() >= nextShoot && !IsInVehicle();
}

void Player::FireBullet()
{
	auto bullet = new Bullet();
	bullet->SetPos(GetPos());
	bullet->SetDamage(25);
	bullet->SetOwner(this);
	bullet->SetSpeed(20);
	bullet->SetDirection(GetDirection());
	bullet->SetSize(25);
}

void Player::AI()
{
	if (!Game::IsValidEntity(targetEntity) || !CanTarget(targetEntity))
	{
		FindTarget();
	}
	else
	{
		Aim(targetEntity->GetPos());

		if (IsInVehicle())
			vehicle->Shoot();
		else
			Shoot();
	}

	if (Game::Time() > nextChangeDirection)
	{
		ImVec2 dir{ Game::Math::RandomFloat(-1, 1), Game::Math::RandomFloat(-1, 1) };
		Game::Math::VecNormalize(dir);
		moveDirection = dir;

		nextChangeDirection = Game::Time() + Game::Math::RandomFloat(aiChangeDirectionTime/2.f, aiChangeDirectionTime);
	}

	if (IsInVehicle())
	{
		vehicle->SetThrottle(Game::Math::Lerp(.25f, vehicle->GetThrottle(), 1));
		vehicle->Drive(moveDirection);
	}
	else if (Game::Math::VecLength(moveDirection) > 0)
		SetVelocity(Game::Math::VecMul(moveDirection, GetWalkSpeed()));
}

Entity* Player::GetTarget()
{
	return targetEntity;
}

void Player::FindTarget()
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

bool Player::CanTarget(Entity* ent)
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
		return ply->team != team && ply->IsAlive();
	}

	case Entity::Unknown:
	case Entity::ProjectileType:
	default:
		break;
	}

	return false;
}

void Player::SetTeam(Team team)
{
	this->team = team;
}

const Player::Team Player::GetTeam()
{
	return team;
}

void Player::SetGod(bool god)
{
	isGod = god;
}

bool Player::IsGod()
{
	return isGod;
}

bool Player::IsAlive()
{
	return GetHealth() > 0;
}

void Player::Damage(int dmg, Entity* attacker)
{
	if (!isGod)
		Entity::Damage(dmg, attacker);

	if (GetHealth() <= 0)
	{
		Kill(attacker);
	}
}

bool Player::ShouldCollide(Entity* otherEnt)
{
	if (otherEnt->GetType() == Entity::Type::ProjectileType && !IsAlive())
		return false;

	return !IsInVehicle();
}

void Player::EnterVehicle(Vehicle* vehicle)
{
	if (Game::IsValidEntity(this->vehicle) || Game::IsValidEntity(vehicle->GetDriver()))
		return;

	this->vehicle = vehicle;
	vehicle->SetDriver(this);
}

void Player::ExitVehicle()
{
	if (!IsInVehicle())
		return;

	vehicle->SetDriver(nullptr);
	vehicle = nullptr;
}

bool Player::IsInVehicle()
{
	return Game::IsValidEntity(vehicle);
}