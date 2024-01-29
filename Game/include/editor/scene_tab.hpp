#pragma once

#include "editor/base_tab.hpp"
#include "engine/ecs.hpp"

class IGraphics;

class SceneTab : public BaseTab
{
public:
	SceneTab(ImGuiID dockID, IGraphics& graphics, entt::entity& selectedEntity, ImGuiWindowFlags_ windowFlags = ImGuiWindowFlags_None, ECS& ecs = ECS::Default());

	// DEVNOTE: Very bad practice to expose this as a static getter, should be fixed at some point.
	static ImVec2 SceneScreenPosition() { return _sceneScreenPosition; }
	static ImVec2 SceneScreenSize() { return _sceneScreenSize; }

protected:
	virtual void DrawContents() override;

private:
	IGraphics& _graphics;
	entt::entity& _selectedEntity;
	ECS& _ecs;
	
	ImGuizmo::OPERATION _currentGizmo{ ImGuizmo::OPERATION::TRANSLATE };
	ImGuizmo::MODE _mode{ ImGuizmo::MODE::LOCAL };

	static ImVec2 _sceneScreenPosition;
	static ImVec2 _sceneScreenSize;


	void DrawHandle(entt::entity entity);
};
