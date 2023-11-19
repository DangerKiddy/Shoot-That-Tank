#pragma once
#include "Entity.h"

class Vehicle : public Entity
{
public:
	Vehicle();
	~Vehicle();

	void Draw(ImDrawList* canvas);
	void Tick();
	void Aim();

	const Entity* GetDriver();
	void SetDriver(Entity* ent);

	bool CanEnter(Entity* ent);
	void SetThrottle(float throttle);
	const float GetThrottle();
	void Drive(const ImVec2 direction);
	void SetTargetAimPos(const ImVec2 pos);
	const ImVec2 GetAimDirection();
	const ImVec2 GetTargetAimPos();

	void Shoot();
	void FireBullet();
	const bool CanShoot();

	bool IsDestroyed();
	void Damage(int dmg, Entity* attacker);

private:
	ImVec2 aimPosition;
	ImVec2 aimDirection;
	ImVec2 moveDirection;
	double nextShoot = 0;
	float shootSpeed = 1.f;
	float throttle = 0.f;

	bool isDestroyed = false;

	Entity* driver = nullptr;

	const float moveSpeed = 2.f;
	const float selfDamageMultiplier = .5f;
};