#include "pch.h"
#include "editor/scene_tab.hpp"
#include "engine/IGraphics.h"
#include "engine/camera.hpp"
#include "engine/ecs.hpp"
#include "engine/transform.hpp"
#include <IconsFontAwesome6.h>

SceneTab::SceneTab(ImGuiID dockID, IGraphics& graphics, entt::entity& selectedEntity, ImGuiWindowFlags_ windowFlags) : BaseTab("Scene", dockID, windowFlags, ImVec2{0.0f, 0.0f}), _graphics(graphics), _selectedEntity(selectedEntity)
{
}

void SceneTab::DrawContents()
{
	ImGuiIO io{ ImGui::GetIO() };
	ImVec2 displaySize{ io.DisplaySize };
	ImVec2 windowSize{ ImGui::GetContentRegionAvail() };

	bool windowFocused = ImGui::IsWindowFocused();

	ImVec2 size{ 108, 34 };
	ImVec2 originalPos{ ImGui::GetCursorScreenPos() };
	ImVec2 pos{ ImGui::GetCursorScreenPos() };
	pos.x += 5.0f;
	pos.y += 5.0f;
	
	ImGui::SetNextWindowPos(pos);

	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
	ImGui::BeginChild("Handles", size, true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	{
		const ImVec2 buttonSize{ 18, 18 };

		ImGui::Columns(3, "Gizmo Toolbar", true);
		bool translationHotkey = ImGui::IsKeyDown(ImGuiKey_Q) && windowFocused;
		if (ImGui::Selectable(ICON_FA_UP_DOWN_LEFT_RIGHT, _currentGizmo == ImGuizmo::TRANSLATE, 0, buttonSize) || translationHotkey)
		{
			_currentGizmo = ImGuizmo::OPERATION::TRANSLATE;
		}

		ImGui::NextColumn();

		bool scalingHotkey = ImGui::IsKeyDown(ImGuiKey_W) && windowFocused;
		if (ImGui::Selectable(ICON_FA_VECTOR_SQUARE, _currentGizmo == ImGuizmo::SCALE, 0, buttonSize) || scalingHotkey)
		{
			_currentGizmo = ImGuizmo::OPERATION::SCALE;
		}

		ImGui::NextColumn();

		bool rotationHotkey = ImGui::IsKeyDown(ImGuiKey_E) && windowFocused;
		if (ImGui::Selectable(ICON_FA_ROTATE, _currentGizmo == ImGuizmo::ROTATE, 0, buttonSize) || rotationHotkey)
		{
			_currentGizmo = ImGuizmo::OPERATION::ROTATE;
		}
	}
	ImGui::EndChild();

	ImGui::SetNextWindowPos(ImVec2{ pos.x + size.x + 8.0f, pos.y });
	size.x = 150;
	ImGui::BeginChild("WorldLocal", size, true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	{
		const ImVec2 buttonSize{ 0.0f, 18.0f };
		ImGui::Columns(2, "WorldLocal Toolbar", true);
		if (ImGui::Selectable(ICON_FA_COMPASS " Local", _mode == ImGuizmo::LOCAL, 0, buttonSize))
		{
			_mode = ImGuizmo::LOCAL;
		}

		ImGui::NextColumn();

		if (ImGui::Selectable(ICON_FA_GLOBE " World", _mode == ImGuizmo::WORLD, 0, buttonSize))
		{
			_mode = ImGuizmo::WORLD;
		}
	}
	ImGui::EndChild();
	ImGui::PopStyleVar();

	float aspectRatioDisplay{ displaySize.x / displaySize.y };
	float aspectRatioWindow{ windowSize.x / windowSize.y };

	ImVec2 newSize{ windowSize };
	ImVec2 dummySpace{ 0.0f, 0.0f };


	if (aspectRatioWindow > aspectRatioDisplay)
	{
		newSize.x = windowSize.y * aspectRatioDisplay;
		dummySpace.x = (windowSize.x - newSize.x) / 2.0f;
	}
	else
	{
		newSize.y = windowSize.x / aspectRatioDisplay;
		dummySpace.y = (windowSize.y - newSize.y) / 2.0f;
	}


	ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
	ImVec2 cursorScreenPos{ ImGui::GetCursorScreenPos() };
	ImGuizmo::SetRect(cursorScreenPos.x, cursorScreenPos.y, newSize.x, newSize.y);
	
	ImVec2 scenePos{originalPos.x + dummySpace.x, originalPos.y + dummySpace.y};
	ImGui::GetWindowDrawList()->AddImage(_graphics.GetRenderTextureSRV(), scenePos,
										 ImVec2{ scenePos.x + newSize.x, scenePos.y + newSize.y },
										 ImVec2(0, 0), ImVec2(1, 1));

	if (_selectedEntity != entt::null)
		DrawHandle(_selectedEntity);
}

void SceneTab::DrawHandle(entt::entity entity)
{
	ECS& ecs{ ECS::Instance() };
	auto cameraMatrixView{ ecs.Registry().view<CameraMatrix>() };
	
	// If no camera present, return early.
	if (cameraMatrixView.empty())
		return;

	// If entity being inspected is the camera, return early.
	if (entity == cameraMatrixView[0])
		return;

	CameraMatrix& cameraMatrix{ ecs.Registry().get<CameraMatrix>(cameraMatrixView[0])};

	Transform& transform = ecs.Registry().get<Transform>(entity);
	const TransformMatrix& transformMatrix = ecs.Registry().get<const TransformMatrix>(entity);
	XMMATRIX transformMat = transformMatrix.worldMatrix;

	bool interacted = ImGuizmo::Manipulate(reinterpret_cast<float*>(cameraMatrix.view.r), reinterpret_cast<float*>(cameraMatrix.projection.r), static_cast<ImGuizmo::OPERATION>(_currentGizmo),
										   _mode, reinterpret_cast<float*>(&transformMat.r));

	if (interacted)
	{
		transformMat = transformMatrix.worldToLocalMatrix * transformMat;

		XMVECTORF32 newPosition;
		XMVECTORF32 newScale;
		XMVECTORF32 newRotation;

		ImGuizmo::DecomposeMatrixToComponents(reinterpret_cast<float*>(&transformMat.r), reinterpret_cast<float*>(newPosition.f),
											  reinterpret_cast<float*>(newRotation.f), reinterpret_cast<float*>(newScale.f));

		newRotation.f[0] = XMConvertToRadians(newRotation.f[0]);
		newRotation.f[1] = XMConvertToRadians(newRotation.f[1]);
		newRotation.f[2] = XMConvertToRadians(newRotation.f[2]);

		transform.position = XMFLOAT2{ newPosition };
		transform.scale = XMFLOAT2{ newScale };
		transform.rotation = newRotation.f[2];
	}
}
