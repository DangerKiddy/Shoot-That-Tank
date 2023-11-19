#pragma once
#include "Entity.h"
#include "Vehicle.h"


#define LOCAL_PLAYER_TEAM Player::Team::Red
#define LOCAL_PLAYER true
class Player : public Entity
{
public:
	enum Team
	{
		None,
		Red,
		Blue
	};

	static Team GetOppositeTeam(Team team);

	Player();
	~Player();

	void Init();
	void InitLocal();
	void Spawn();
	void Kill(Entity* attacker = nullptr);

	void SetupColors();

	void Draw(ImDrawList* canvas);
	void Tick();

	void AI();

	void FindTarget();
	bool CanTarget(Entity* ent);
	Entity* GetTarget();

	void Control();

	void Aim(ImVec2 pos);
	void Shoot();
	bool CanShoot();
	void FireBullet();

	const ImVec2 GetMoveDirection();
	const ImVec2 GetDirection();
	void SetDirection(ImVec2 dir);

	const bool IsLocal();
	const float GetWalkSpeed();

	const Team GetTeam();
	void SetTeam(Team team);

	void SetGod(bool god);
	bool IsGod();

	bool IsAlive();
	void Damage(int dmg, Entity* attacker);
	bool ShouldCollide(Entity* otherEnt);

	void EnterVehicle(Vehicle* vehicle);
	void ExitVehicle();
	bool IsInVehicle();

private:
	bool isGod = false;
	bool isLocal = false;
	ImVec2 lookDirection;
	ImVec2 moveDirection;
	double nextShoot = 0;
	float shootSpeed = 1.f;
	double respawnAtTime = 0;

	Vehicle* vehicle;

	// AI
	Entity* targetEntity;
	double nextChangeDirection = 0;

	const float walkSpeed = 2.f;
	const float aiChangeDirectionTime = 3.f;
	const float respawnTime = 3.f;

	Team team;
};