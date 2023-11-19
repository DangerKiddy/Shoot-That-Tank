#pragma once
#include "Entity.h"

struct HitResult
{
	Entity* hitEntity;
	ImVec2 hitPos;
	bool hit;
};

class Bullet : public Entity
{
public:
	Bullet();
	~Bullet();

	void SetDamage(int dmg);
	void SetSpeed(float speed);
	void SetDirection(ImVec2 dir);
	void SetOwner(Entity* ent);
	void SetColor(ImU32 color);

	void Tick();
	HitResult CheckHit();
	HitResult CheckWorldHit();

	void Draw(ImDrawList* canvas);
	void DrawQuad(ImDrawList* canvas, ImVec2 pos, ImU32 color);

	void CacheBoundsFromPosition(ImVec2 pos);
	ImVec2* GetBoundsFromPosition(ImVec2 pos);

	bool ShouldCollide(Entity* otherEnt);

private:
	int damage;
	float speed;
	ImVec2 dir;
	Entity* owner;

	ImVec2 cachedBounds[4];

	ImU32 ghostColor;
	ImU32 ghostColor2;
};
