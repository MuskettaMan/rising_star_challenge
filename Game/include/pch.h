// Entt
#include <entt/entt.hpp>

// Windows
#include <Windows.h>
#include "wrl/client.h"

using Microsoft::WRL::ComPtr;

// Direct X
#include <d3dcommon.h>
#include <d3d11.h>
#include <D3DCompiler.h>
#include <DDSTextureLoader.h>
#include <DirectXMath.h>

#include <XInput.h>

#include <directxmath.h>
using namespace DirectX;

// Standard Library
#include <ctime>
#include <math.h>
#include <iostream>
#include <memory>
#include <array>
#include <vector>
#include <string>

// ImGui
#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"

// Visit struct
#include "visit_struct/visit_struct.hpp"

// Else
#include "stduuid/uuid.h"

// Constants
constexpr bool ENABLE_EDITOR = true;

// ECS
//#include "engine/ecs.hpp"
