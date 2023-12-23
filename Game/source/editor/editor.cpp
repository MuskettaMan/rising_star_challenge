#include "pch.h"
#include "editor/editor.hpp"
#include "editor/editor_util.hpp"
#include "editor/editor_environment.hpp"

#include <imgui.h>
#include "imgui/IconsFontAwesome6.h"

#include "engine/IGraphics.h"

Editor::Editor(IGraphics& graphics) : _graphics(graphics)
{
	if (ENABLE_EDITOR)
	{
        std::string segoeuiPath = "assets\\fonts\\segoeui\\segoeui.ttf";
        std::string fontAwesomePath = std::string("assets\\fonts\\font_awesome\\") + FONT_ICON_FILE_NAME_FAR;
        ImGuiIO& io = ImGui::GetIO();
        ImFontConfig config{};

        float baseFontSize = 18;
        float iconFontSize = baseFontSize;

        io.Fonts->AddFontFromFileTTF(segoeuiPath.c_str(), baseFontSize, &config);

        const ImWchar iconRanges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
        ImFontConfig iconConfig;
        iconConfig.MergeMode = true;
        iconConfig.PixelSnapH = true;
        iconConfig.GlyphMinAdvanceX = iconFontSize;
        ImFont* fontAwesome = io.Fonts->AddFontFromFileTTF(fontAwesomePath.c_str(), iconFontSize, &iconConfig, iconRanges);

        io.Fonts->Build();

        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        ImGui::GetStyle().WindowRounding = 10.0f;
        ImGui::GetStyle().FrameRounding = 3.0f;

        ImVec4 normal = HexToVec4(0xe8b31aFF);
        ImVec4 hover = HexToVec4(0xdf8c10FF);
        ImVec4 active = HexToVec4(0xd4660aFF);

        ImVec4 black = HexToVec4(0x1a1a1aFF);
        ImVec4 grey = HexToVec4(0x8f8f8fFF);
        ImVec4 middleGrey = HexToVec4(0x6e6e6eFF);
        ImVec4 darkGrey = HexToVec4(0x5e5e5eFF);
        ImVec4 darkerGrey = HexToVec4(0x474747FF);


        ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive] = black;
        ImGui::GetStyle().Colors[ImGuiCol_TitleBg] = black;
        ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = black;
        ImGui::GetStyle().Colors[ImGuiCol_ChildBg] = black;

        ImGui::GetStyle().Colors[ImGuiCol_Button] = middleGrey;
        ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = darkGrey;
        ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = darkerGrey;

        ImGui::GetStyle().Colors[ImGuiCol_Header] = darkerGrey;
        ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered] = darkGrey;
        ImGui::GetStyle().Colors[ImGuiCol_HeaderActive] = middleGrey;

        ImGui::GetStyle().Colors[ImGuiCol_DragDropTarget] = active;

        ImGui::GetStyle().Colors[ImGuiCol_SliderGrab] = middleGrey;
        ImGui::GetStyle().Colors[ImGuiCol_SliderGrabActive] = darkerGrey;

        ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = HexToVec4(0x3d3d3d88);
        ImGui::GetStyle().Colors[ImGuiCol_FrameBgHovered] = HexToVec4(0x3d3d3dAA);
        ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive] = HexToVec4(0x3d3d3dFF);

        ImGui::GetStyle().Colors[ImGuiCol_ResizeGrip] = normal;
        ImGui::GetStyle().Colors[ImGuiCol_ResizeGripHovered] = hover;
        ImGui::GetStyle().Colors[ImGuiCol_ResizeGripActive] = active;

        ImGui::GetStyle().Colors[ImGuiCol_Tab] = darkerGrey;
        ImGui::GetStyle().Colors[ImGuiCol_TabUnfocusedActive] = middleGrey;
        ImGui::GetStyle().Colors[ImGuiCol_TabUnfocused] = darkerGrey;
        ImGui::GetStyle().Colors[ImGuiCol_TabHovered] = middleGrey;
        ImGui::GetStyle().Colors[ImGuiCol_TabActive] = grey;

        ImGui::GetStyle().Colors[ImGuiCol_SeparatorActive] = active;
        ImGui::GetStyle().Colors[ImGuiCol_SeparatorHovered] = hover;
		
		_editorEnvironment = std::make_unique<EditorEnvironment>(_graphics);
	}
}

Editor::~Editor() = default;

void Editor::Update()
{
	if(ENABLE_EDITOR)
		_editorEnvironment->Update();
}
