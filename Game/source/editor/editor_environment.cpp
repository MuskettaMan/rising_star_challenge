#include "pch.h"
#include "editor/editor_environment.hpp"
#include "editor/scene_tab.hpp"
#include "editor/inspector_tab.hpp"
#include "editor/hierarchy_tab.hpp"
#include "engine/IGraphics.h"

constexpr int32_t ROOT_WINDOW_FLAGS = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNavFocus;

EditorEnvironment::EditorEnvironment(IGraphics& graphics) : _graphics(graphics), _tabs()
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

		ImGui::Dummy(ImVec2{0, 20});


		if (!_dockSpaceInitialized)
		{
			ImGuiID sceneID;
			ImGuiID hierarchyID;
			ImGuiID inspectorID;

			_dockRootID = ImGui::GetID("Dockspace");
			ImGui::DockBuilderAddNode(_dockRootID, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_DockSpace);
			ImGui::DockBuilderSetNodeSize(_dockRootID, ImGui::GetIO().DisplaySize);
			ImGui::DockBuilderSplitNode(_dockRootID, ImGuiDir_Left, 0.3f, &hierarchyID, &sceneID);
			ImGui::DockBuilderSplitNode(hierarchyID, ImGuiDir_Left, 0.5f, &hierarchyID, &inspectorID);
			ImGui::DockBuilderFinish(_dockRootID);

			_tabs.push_back(std::make_unique<SceneTab>(sceneID, _graphics, _selectedEntity, ImGuiWindowFlags_NoScrollbar));
			_tabs.push_back(std::make_unique<HierarchyTab>(hierarchyID, _selectedEntity));
			_tabs.push_back(std::make_unique<InspectorTab>(inspectorID, _selectedEntity));

			_dockSpaceInitialized = true;
		}

		ImGui::DockSpace(_dockRootID);

		for (auto& tab : _tabs)
		{
			ImGui::SetNextWindowDockID(tab->GetDockID(), ImGuiCond_Once);
			tab->Draw();
		}

	}
	ImGui::End();
}
