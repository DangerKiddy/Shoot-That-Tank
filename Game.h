#pragma once
#include "UI.h"
#include "Entity.h"
#include "Player.h"
#include <unordered_map>
#include <vector>

struct SphereIntersection
{
	bool intersect;
	float x;
	float y;
};

namespace Game
{
	void Init();
	void ImGuiLoop();

	void InitEntities();

	void MainLoop();
	void EntitiesLoop(ImDrawList* canvas);

	bool IsKeyPressed(int key);
	bool IsKeyDown(int key);

	ImVec2 GetWorldCenter();
	ImVec2 GetRandomWorldPos();

	void RegisterEntity(Entity* ent);
	void ForgetEntity(Entity* ent);
	double Time();

	bool IsPointOutsideWorld(ImVec2 pos);
	const SphereIntersection RaySphereIntersection(const ImVec2 rayOrigin, const ImVec2 rayDir, const ImVec2 sphereOrigin, const float sphereRadius);

	inline extern int lastEntityId = 0;
	inline extern std::unordered_map<int, Entity*> entities{};

	bool IsValidEntity(const Entity* ent);

	inline extern Player* LocalPlayer = nullptr;
	inline extern bool isInteractiveHovered = false;
}