#include "Entity.h"
#include "Math.h"
#include "Game.h"
#include "Events.h"

Entity::Entity()
{
	id = -1;

	SetColor(IM_COL32(255, 255, 255, 255));
}

void Entity::SetId(int id)
{
	if (this->id == -1)
		this->id = id;
}

int Entity::ID()
{
	return id;
}

Entity::~Entity()
{
	Game::ForgetEntity(this);
}

void Entity::Init()
{
	SetPos(ImVec2{ 0,0 });

	Game::RegisterEntity(this);
}

void Entity::Draw(ImDrawList* canvas)
{
	canvas->AddCircle(GetPos(), GetSize(), color, 10);
}

void Entity::DrawHealth(ImDrawList* canvas)
{
	float hpPerc = fmaxf(0, GetHealth() / 100.f);
	canvas->AddLine(Game::Math::VecAdd(position, ImVec2{ size + 5, size }), Game::Math::VecAdd(position, ImVec2{ size + 5, -size }), IM_COL32(0, 0, 0, 100), 3);
	canvas->AddLine(Game::Math::VecAdd(position, ImVec2{ size + 5, size }), Game::Math::VecAdd(position, ImVec2{ size + 5, -size + (size * 2) * (1 - hpPerc) }), IM_COL32(0, 255, 0, 255), 3);
}

void Entity::Tick()
{
	SetPos(Game::Math::VecAdd(position, velocity));

	SetVelocity(Game::Math::VecMul(velocity, stiffness));
}

const float Entity::GetStiffness()
{
	return stiffness;
}

void Entity::SetStiffness(float stiffness)
{
	this->stiffness = stiffness;
}

void Entity::SetVelocity(ImVec2 vel)
{
	velocity = vel;
}

const ImVec2 Entity::GetVelocity()
{
	return velocity;
}

const float Entity::GetSize()
{
	return size;
}

void Entity::SetSize(float size)
{
	this->size = size;
}

void Entity::SetPos(ImVec2 pos)
{
	position = pos;

	position.x = fmaxf(size, fminf(WIDTH - size, position.x));
	position.y = fmaxf(size, fminf(HEIGHT - size, position.y));
}

const ImVec2 Entity::GetPos()
{
	return position;
}

const Entity::Type Entity::GetType()
{
	return type;
}

const bool Entity::IsPlayer()
{
	return GetType() == Type::PlayerType;
}

const bool Entity::IsNPC()
{
	return GetType() == Type::NPCType;
}

void Entity::SetColor(ImU32 col)
{
	color = col;
}

const ImU32 Entity::GetColor()
{
	return color;
}

const int Entity::GetHealth()
{
	return health;
}

void Entity::SetHealth(int hp)
{
	health = hp;
}

void Entity::Damage(int dmg, Entity* attacker)
{
	SetHealth(health - dmg);

	Events::OnEntityTookDamage(this, attacker, dmg);
}

bool Entity::ShouldCollide(Entity* otherEnt)
{
	return true;
}