#include <iostream>
#include <Windows.h>
#include <unordered_map>
#include <string>

#include "UI.h"
#include "Game.h"
#include "Math.h"

#include "EntityFactory.h"


int main()
{
	Game::Init();
}

void Game::Init()
{
	UI::InitImGui();

    InitEntities();

	ImGuiLoop();
}

bool Game::IsKeyPressed(int key)
{
    return GetKeyState(key) & 0x1;
}

bool Game::IsKeyDown(int key)
{
    return GetKeyState(key) & 0x800;
}

void Game::RegisterEntity(Entity* ent)
{
    lastEntityId++;

    entities[lastEntityId] = ent;
    ent->SetId(lastEntityId);
}

void Game::ForgetEntity(Entity* ent)
{
    entities.erase(ent->ID());
}

double Game::Time()
{
    return (double)clock() / (double)CLOCKS_PER_SEC;
}

void InitializeLocalPlayer()
{
    auto ply = EntityFactory::CreatePlayer(LOCAL_PLAYER);

    ply->InitLocal();
    ply->SetTeam(LOCAL_PLAYER_TEAM);
    ply->Spawn();

    Game::LocalPlayer = ply;
}

void CreateEnemyEntities(bool includeNpcs = false)
{
    auto veh = EntityFactory::CreateVehicle();
    veh->Init();
    veh->SetPos(ImVec2{ 200,700 });

    auto oppositeTeam = Player::GetOppositeTeam(LOCAL_PLAYER_TEAM);

    int driver = Game::Math::RandomInt(0, 1);
    for (int i = 0; i < 2; i++)
    {
        auto ply = EntityFactory::CreatePlayer();
        ply->SetTeam(oppositeTeam);

        ply->Spawn();
        ply->SetPos(Game::GetRandomWorldPos());

        if (i == driver)
            ply->EnterVehicle(veh);
    }

    auto npc = EntityFactory::CreateNPC();
    npc->Init();
    npc->SetPos(Game::GetRandomWorldPos());
    npc->SetTeam(oppositeTeam);
}

void CreateFriendlyEntities(bool includeNpcs = false)
{
    auto veh = EntityFactory::CreateVehicle();
    veh->Init();
    veh->SetPos(ImVec2{ 700,700 });

    auto sameTeam = LOCAL_PLAYER_TEAM;

    int driver = Game::Math::RandomInt(0, 1);
    for (int i = 0; i < 1; i++)
    {
        auto ply = EntityFactory::CreatePlayer();
        ply->SetTeam(sameTeam);

        ply->Spawn();
        ply->SetPos(Game::GetRandomWorldPos());

        if (i == driver)
            ply->EnterVehicle(veh);
    }

    auto npc = EntityFactory::CreateNPC();
    npc->Init();
    npc->SetPos(Game::GetRandomWorldPos());
    npc->SetTeam(sameTeam);
}

void CreateTeamEntities(bool includeNpcs = false)
{
    CreateEnemyEntities(includeNpcs);
    CreateFriendlyEntities(includeNpcs);
}

void Game::InitEntities()
{
    CreateTeamEntities(true);

    auto ent = EntityFactory::CreateEntity();
    ent->Init();
    ent->SetPos(ImVec2{ 200, 200 });

    InitializeLocalPlayer();
}

static const ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus;
void Game::ImGuiLoop()
{
    bool shouldExit = false;
    while (!glfwWindowShouldClose(UI::window))
    {
        if (shouldExit)
            break;

        shouldExit = IsKeyPressed('Q');

        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        auto& io = ImGui::GetIO();

        ImGui::SetNextWindowPos(ImVec2(0.f, 0.f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);

        if (!ImGui::Begin("Window", nullptr, windowFlags))
        {
            ImGui::End();

            return;
        }

        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImGui::GetStyleColorVec4(ImGuiCol_Button));

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Debug"))
            {
                if (Game::IsValidEntity(Game::LocalPlayer))
                {
                    if (ImGui::MenuItem("God Mode", Game::LocalPlayer->IsGod() ? "Currently ON" : "Currently OFF"))
                    {
                        Game::LocalPlayer->SetGod(!Game::LocalPlayer->IsGod());
                    }
                }

                ImGui::Separator();
                if (ImGui::MenuItem("Close", "Q"))
                    break;
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        MainLoop();

        ImGui::PopStyleColor();
        ImGui::End();

        ImGui::Render();

        glViewport(0, 0, WIDTH, HEIGHT);
        glClearColor(.45f, .45f, .45f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(UI::window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}

void Game::MainLoop()
{
    ImDrawList* canvas = ImGui::GetWindowDrawList();

    EntitiesLoop(canvas);
}

void Game::EntitiesLoop(ImDrawList* canvas)
{
    for (auto it = std::begin(entities); it != std::end(entities);)
    {
        auto ent = it->second;
        ++it;

        if (IsValidEntity(ent))
        {
            ent->Draw(canvas);
            ent->Tick();
        }
    }
    isInteractiveHovered = false;
}

ImVec2 Game::GetWorldCenter()
{
    return ImVec2{ WIDTH / 2.f, HEIGHT / 2.f };
}

ImVec2 Game::GetRandomWorldPos()
{
    return ImVec2{Math::RandomFloat(50, WIDTH - 50), Math::RandomFloat(50, HEIGHT - 50)};
}

bool Game::IsPointOutsideWorld(ImVec2 pos)
{
    return pos.x < 0 || pos.x > WIDTH || pos.y < 0 || pos.y > HEIGHT;
}

const SphereIntersection Game::RaySphereIntersection(const ImVec2 rayOrigin, const ImVec2 rayDir, const ImVec2 sphereOrigin, const float sphereRadius)
{
    SphereIntersection res{};
    res.intersect = false;

    ImVec2 oc = Math::VecSub(rayOrigin, sphereOrigin);
    float b = Math::VecDot(oc, rayDir);
    float c = Math::VecDot(oc, oc) - sphereRadius * sphereRadius;
    float h = b * b - c;

    if (h < 0.0)
        return res; // no intersection

    h = sqrt(h);

    res.x = -b - h;
    res.y = -b + h;
    res.intersect = true;

    return res;
}

bool Game::IsValidEntity(const Entity* ent)
{
    return ent != nullptr;
}