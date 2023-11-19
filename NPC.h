#pragma once
#include "Entity.h"
#include "Player.h"

class NPC : public Entity
{
public:
	NPC();
	~NPC();

	void Draw(ImDrawList* canvas);
	void Tick();

	void SetupColors();

	void AI();

	void FindTarget();
	bool CanTarget(Entity* ent);
	Entity* GetTarget();

	void Aim(ImVec2 pos);
	void Shoot();
	bool CanShoot();
	void FireBullet();

	const Player::Team GetTeam();
	void SetTeam(Player::Team team);

	const float GetWalkSpeed();

	bool IsAlive();
	void Damage(int dmg, Entity* attacker);
	bool ShouldCollide(Entity* otherEnt);

	void Spawn();
	void Kill(Entity* attacker = nullptr);

private:
	ImVec2 lookDirection;
	ImVec2 moveDirection;
	double nextShoot = 0;
	float shootSpeed = 1.5f;
	double respawnAtTime = 0;

	Entity* targetEntity;
	double nextChangeDirection = 0;

	const float walkSpeed = 1.5f;
	const float aiChangeDirectionTime = 3.f;
	const float respawnTime = 3.f;

	Player::Team team;
};