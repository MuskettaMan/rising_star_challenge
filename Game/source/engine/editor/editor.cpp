#include "pch.h"
#include "engine/editor/editor.hpp"
#include <imgui.h>
#include "engine/IGraphics.h"

Editor::Editor(IGraphics& graphics) : _graphics(graphics)
{

}

void Editor::Update()
{
	ImGui::Begin("Test");
	ImGui::Image(_graphics.GetBackBuffer(), ImVec2{ 1920, 1080 });
	ImGui::End();
}
