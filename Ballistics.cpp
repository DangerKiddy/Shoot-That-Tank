#include "Ballistics.h"
#include "Game.h"
#include "Math.h"

Bullet::Bullet()
{
	Init();
	SetColor(IM_COL32(255, 255, 0, 255));

	for (int i = 0; i < 4; i++)
	{
		cachedBounds[i] = ImVec2{ 0,0 };
	}

	type = Type::ProjectileType;
}

void Bullet::SetColor(ImU32 color)
{
	this->color = color;

	ImVec4 col = ImGui::ColorConvertU32ToFloat4(color);
	col.w = .5f;
	ghostColor = ImGui::ColorConvertFloat4ToU32(col);

	col.w = .25f;
	ghostColor2 = ImGui::ColorConvertFloat4ToU32(col);
}

Bullet::~Bullet()
{
}

void Bullet::SetDamage(int dmg)
{
	damage = dmg;
}
void Bullet::SetSpeed(float speed)
{
	this->speed = speed;
}
void Bullet::SetDirection(ImVec2 dir)
{
	this->dir = dir;
}
void Bullet::SetOwner(Entity* ent)
{
	owner = ent;
}

void Bullet::Tick()
{
	Entity::Tick();

	SetVelocity(Game::Math::VecMul(dir, speed));

	CacheBoundsFromPosition(GetPos());
	auto res = CheckHit();

	if (res.hit)
	{
		delete this;
	}
}

void Bullet::CacheBoundsFromPosition(ImVec2 pos)
{
	auto bounds = GetBoundsFromPosition(pos);

	for (int i = 0; i < 4; i++)
	{
		cachedBounds[i] = bounds[i];
	}
}

ImVec2* Bullet::GetBoundsFromPosition(ImVec2 pos)
{
	ImVec2 bounds[4]{};

	auto left = Game::Math::VecCross(dir);
	Game::Math::VecNormalize(left);

	auto right = Game::Math::VecMul(left, -1);

	auto leftTop = Game::Math::VecAdd(pos, Game::Math::VecMul(left, size / 1.5f));
	leftTop = Game::Math::VecAdd(leftTop, Game::Math::VecMul(dir, size * 2));

	auto rightTop = Game::Math::VecAdd(pos, Game::Math::VecMul(right, size / 1.5f));
	rightTop = Game::Math::VecAdd(rightTop, Game::Math::VecMul(dir, size * 2));

	auto leftBottom = Game::Math::VecAdd(pos, Game::Math::VecMul(left, size / 1.5f));
	leftBottom = Game::Math::VecAdd(leftBottom, Game::Math::VecMul(dir, -size * 2));

	auto rightBottom = Game::Math::VecAdd(pos, Game::Math::VecMul(right, size / 1.5f));
	rightBottom = Game::Math::VecAdd(rightBottom, Game::Math::VecMul(dir, -size * 2));

	bounds[0] = leftTop;
	bounds[1] = rightTop;
	bounds[2] = rightBottom;
	bounds[3] = leftBottom;

	return bounds;
}

HitResult Bullet::CheckHit()
{
	auto worldHitCheck = CheckWorldHit();

	if (worldHitCheck.hit)
		return worldHitCheck;

	HitResult res{};
	res.hit = false;
	res.hitEntity = nullptr;

	auto pos = GetPos();
	for (auto it = std::begin(Game::entities); it != std::end(Game::entities);)
	{
		auto ent = it->second;
		++it;

		if (!Game::IsValidEntity(ent) || ent->ID() == ID() || (Game::IsValidEntity(owner) && ent->ID() == owner->ID()) || !ShouldCollide(ent))
			continue;

		auto rayDir = Game::Math::VecMul(dir, .5f);
		auto sphereRes = Game::RaySphereIntersection(pos, rayDir, ent->GetPos(), ent->GetSize());
		if (sphereRes.intersect && ent->ShouldCollide(this))
		{
			res.hit = true;
			res.hitEntity = ent;

			ent->Damage(damage, owner);

			break;
		}
	}

	return res;
}

HitResult Bullet::CheckWorldHit()
{
	HitResult res{};
	res.hit = false;
	res.hitEntity = nullptr;
	res.hitPos = GetPos();

	for (int i = 0; i < 2; i++)
	{
		if (Game::IsPointOutsideWorld(cachedBounds[i]))
		{
			res.hit = true;
			res.hitPos = Game::Math::VecClamp(cachedBounds[i]);

			break;
		}
	}

	return res;
}

void Bullet::Draw(ImDrawList* canvas)
{
	auto pos = GetPos();
	auto prevPos = GetPos();
	// move previous pose back a little bit to make ghosting more noticable
	prevPos = Game::Math::VecSub(prevPos, Game::Math::VecMul(dir, 20));
	DrawQuad(canvas, prevPos, ghostColor2);

	// move it forward for next ghost effect
	prevPos = Game::Math::VecAdd(prevPos, Game::Math::VecMul(dir, 10));
	DrawQuad(canvas, prevPos, ghostColor);

	DrawQuad(canvas, pos, color);
}

void Bullet::DrawQuad(ImDrawList* canvas, ImVec2 pos, ImU32 color)
{
	auto bounds = GetBoundsFromPosition(pos);

	canvas->AddQuadFilled(bounds[0], bounds[1], bounds[2], bounds[3], color);
}

bool Bullet::ShouldCollide(Entity* otherEnt)
{
	if (otherEnt->IsPlayer())
		return ((Player*)otherEnt)->IsAlive();
	else
		return otherEnt->GetType() != Entity::Type::ProjectileType;
}