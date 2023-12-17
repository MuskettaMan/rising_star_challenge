#include "pch.h"
#include "editor/editor_environment.hpp"
#include "engine/IGraphics.h"

constexpr int32_t ROOT_WINDOW_FLAGS = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNavFocus;

EditorEnvironment::EditorEnvironment(IGraphics& graphics) : _graphics(graphics)
{
}

EditorEnvironment::~EditorEnvironment() = default;

void EditorEnvironment::Update()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
	ImGui::SetNextWindowPos(ImVec2{ 0, 0 }, ImGuiCond_Once);
	ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);
	ImGui::Begin("Root Window", nullptr, ROOT_WINDOW_FLAGS);
	{
		ImGui::PopStyleVar();

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Nothing yet!"))
				{

				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}


		if (!_dockSpaceInitialized)
		{
			_dockRootID = ImGui::GetID("Dockspace");
			ImGui::DockBuilderAddNode(_dockRootID, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_DockSpace);
			ImGui::DockBuilderSetNodeSize(_dockRootID, ImGui::GetIO().DisplaySize);
			ImGui::DockBuilderSplitNode(_dockRootID, ImGuiDir_Left, 0.3f, &_hierarchyID, &_gameID);
			ImGui::DockBuilderSplitNode(_hierarchyID, ImGuiDir_Left, 0.5f, &_hierarchyID, &_inspectorID);
			ImGui::DockBuilderFinish(_dockRootID);

			_dockSpaceInitialized = true;
		}

		ImGui::DockSpace(_dockRootID);

		ImGui::SetNextWindowDockID(_hierarchyID, ImGuiCond_Once);
		ImGui::Begin("Hierarchy");
		ImGui::End();

		ImGui::SetNextWindowDockID(_inspectorID, ImGuiCond_Once);
		ImGui::Begin("Inspector");
		ImGui::End();

		ImGui::SetNextWindowDockID(_gameID, ImGuiCond_Once);
		ImGui::Begin("Game");
		ImVec2 windowSize{ ImGui::GetWindowSize() };

		// Determine change in aspect ratio between imgui window and actual Windows window.
		float change{ 1.0f - (io.DisplaySize.x - windowSize.x) / io.DisplaySize.x };

		// Apply dummy space to center the game.
		ImGui::Dummy(ImVec2{ 0.0f, (windowSize.y - windowSize.y * change) / 2.0f });
		ImGui::Image(_graphics.GetRenderTextureSRV(), ImVec2{ windowSize.x, windowSize.y * change });
		ImGui::End();

	}
	ImGui::End();
}
