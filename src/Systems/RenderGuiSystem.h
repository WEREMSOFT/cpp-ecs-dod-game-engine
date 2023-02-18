#pragma once

#include "../ECS/ECS.h"
#include <imgui/imgui.h>
#include <imgui/imgui_sdl.h>
#include <imgui/imgui_impl_sdl2.h>

class RenderGuiSystem: public System
{
	int enemyXPos = 0;
	int enemyYPos = 0;
	public:
		RenderGuiSystem() = default;

	void Update(const std::unique_ptr<Registry>& registry)
	{
		ImGui::NewFrame();
		if(ImGui::Begin("Spawn Enemies"))
		{
			ImGui::InputInt("x position", &enemyXPos);
			ImGui::InputInt("y position", &enemyYPos);
			if(ImGui::Button("Create new enemy"))
			{
				Entity enemy = registry->CreateEntity();
				enemy.AddComponent<TransformComponent>(glm::vec2(50., 50.), glm::vec2(1., 1.), 0.);
				enemy.AddComponent<RigidBodyComponent>(glm::vec2(10., 0.));
				enemy.AddComponent<SpriteComponent>("tank-image", 32, 32, 1);
				enemy.AddComponent<BoxColliderComponent>(32, 32);
				enemy.AddComponent<ProjectileEmitterComponent>(glm::vec2(100, 0), 1000, 5000, 10, false);
				enemy.AddComponent<HealthComponent>(100);
				enemy.Group("enemies");

			}
		}
		ImGui::End();
		ImGui::Render();
		ImGuiSDL::Render(ImGui::GetDrawData());
	}
};