#pragma once

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#define WIDTH 1200
#define HEIGHT 900
namespace Game
{
	namespace UI
	{
		inline static GLFWwindow* window = nullptr;

		inline static bool InitImGui()
		{
            if (!glfwInit()) {
                std::cerr << "GLFW init failed" << std::endl;

                return false;
            }

            glfwWindowHint(GLFW_RESIZABLE, false);
            window = glfwCreateWindow(WIDTH, HEIGHT, "Shoot That Tank!", nullptr, nullptr);
            if (!window) {
                std::cerr << "GLFW create window failed" << std::endl;
                glfwTerminate();

                return false;
            }

            glfwMakeContextCurrent(window);
            if (glewInit() != GLEW_OK) {
                std::cerr << "GLEW init failed" << std::endl;
                glfwTerminate();

                return false;
            }

            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            ImGui::StyleColorsDark();

            ImGui_ImplGlfw_InitForOpenGL(window, true);
            ImGui_ImplOpenGL3_Init("#version 330 core");

            return true;
		}
	}
}