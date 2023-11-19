#include "Vehicle.h"
#include "Ballistics.h"
#include "Math.h"
#include "Game.h"

#include <string>

Vehicle::Vehicle()
{
	SetStiffness(.95f);
	SetHealth(100);

	type = Type::VehicleType;
}
Vehicle::~Vehicle()
{

}

const Entity* Vehicle::GetDriver()
{
	return driver;
}

void Vehicle::SetDriver(Entity* driver)
{
	this->driver = driver;
}

void Vehicle::SetThrottle(float throttle)
{
	this->throttle = fmaxf(0, fminf(1, throttle));
}

const float Vehicle::GetThrottle()
{
	return throttle;
}

void Vehicle::Drive(const ImVec2 direction)
{
	SetVelocity(Game::Math::VecMul(direction, throttle));
}

void Vehicle::SetTargetAimPos(const ImVec2 pos)
{
	aimPosition = pos;
}

const ImVec2 Vehicle::GetTargetAimPos()
{
	return aimPosition;
}

const ImVec2 Vehicle::GetAimDirection()
{
	return aimDirection;
}

void Vehicle::Shoot()
{
	if (CanShoot())
	{
		FireBullet();
		nextShoot = Game::Time() + shootSpeed;
	}
}

void Vehicle::FireBullet()
{
	auto bullet = new Bullet();
	bullet->SetPos(GetPos());
	bullet->SetDamage(50);
	bullet->SetOwner(this);
	bullet->SetSpeed(25);
	bullet->SetDirection(aimDirection);
	bullet->SetSize(30);
}

const bool Vehicle::CanShoot()
{
	return Game::Time() >= nextShoot;
}

void Vehicle::Tick()
{
	Entity::Tick();

	Aim();
}

void Vehicle::Aim()
{
	ImVec2 targetDir = Game::Math::VecSub(aimPosition, GetPos());
	Game::Math::VecNormalize(targetDir);

	aimDirection = Game::Math::VecApproach(.01f, aimDirection, targetDir);
	Game::Math::VecNormalize(aimDirection);
}

void Vehicle::Draw(ImDrawList* canvas)
{
	auto minMax = ImVec2{ size, size };
	auto min = Game::Math::VecSub(position, minMax);
	auto max = Game::Math::VecAdd(position, minMax);

	auto color = IM_COL32(200, 200, 200, 255);
	if (Game::IsValidEntity(driver))
		color = driver->GetColor();

	canvas->AddRect(min, max, color);
	canvas->AddLine(position, Game::Math::VecAdd(position, Game::Math::VecMul(aimDirection, size*2.f)), color);

	Entity::DrawHealth(canvas);

	std::string txt = std::to_string(ID());
	ImGui::SetCursorPos(min);
	if (ImGui::Button(txt.c_str(), Game::Math::VecMul(minMax, 2)))
	{
		if (Game::LocalPlayer->IsInVehicle())
			Game::LocalPlayer->ExitVehicle();
		else if (CanEnter(Game::LocalPlayer))
			Game::LocalPlayer->EnterVehicle(this);
	}

	if (ImGui::IsItemHovered())
	{
		Game::isInteractiveHovered = true;
	}
}

bool Vehicle::CanEnter(Entity* ent)
{
	return !isDestroyed && !Game::IsValidEntity(driver);
}

void Vehicle::Damage(int dmg, Entity* attacker)
{
	dmg = dmg * selfDamageMultiplier;
	Entity::Damage(dmg, attacker);

	if (GetHealth() <= 0 && !isDestroyed)
	{
		isDestroyed = true;

		if (Game::IsValidEntity(driver))
		{
			if (driver->IsPlayer())
				((Player*)driver)->ExitVehicle();
		}
	}
}

bool Vehicle::IsDestroyed()
{
	return isDestroyed;
}