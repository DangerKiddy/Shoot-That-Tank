#pragma once
#include "UI.h"

class Entity
{
public:
	enum Type
	{
		Unknown,
		VehicleType,
		NPCType,
		PlayerType,
		ProjectileType,
	};

	Entity();
	~Entity();

	void SetPos(ImVec2 pos);
	const ImVec2 GetPos();

	const Type GetType();
	const bool IsPlayer();
	const bool IsNPC();

	const ImU32 GetColor();
	virtual void SetColor(ImU32 col);

	const int GetHealth();
	void SetHealth(int hp);

	const float GetSize();
	virtual void SetSize(float size);

	virtual void Init();
	virtual void Draw(ImDrawList* canvas);
	virtual void DrawHealth(ImDrawList* canvas);
	virtual void Tick();

	virtual void SetVelocity(ImVec2 vel);
	const ImVec2 GetVelocity();

	const float GetStiffness();
	void SetStiffness(float stiffness);

	void SetId(int id);
	int ID();

	virtual bool ShouldCollide(Entity* otherEnt);
	
	virtual void Damage(int dmg, Entity* attacker);

protected:
	Type type = Unknown;
	ImVec2 position;
	ImVec2 velocity;
	ImU32 color;
	float size = 15.f;

private:
	bool removing = false;
	int health;
	int id;

	float stiffness = .95f;
};