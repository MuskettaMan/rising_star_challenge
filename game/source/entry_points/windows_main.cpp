// The main Windows / DirectX Graphics / XInput entry point for Star Applications

#define WIN32_LEAN_AND_MEAN

#include "pch.h"
#include "engine/Implementation/directx11/dx11_graphics.hpp"
#include "engine/Implementation/XInput/dx_input.hpp"
#include "engine/IApplication.h"
#include <editor/editor.hpp>

const char WindowClassName[] = "Star";
const char WindowTitle[] = "Search for a Star 2024";
const int WindowWidth = 1920;
const int WindowHeight = 1080;

bool doResize = false;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
std::unique_ptr<IApplication> GetApplication(std::shared_ptr<IGraphics> Graphics, std::shared_ptr<IInput> Input);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ImGui_ImplWin32_EnableDpiAwareness();

	WNDCLASSEX wc;
	HWND hwnd;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WindowClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx( WS_EX_CLIENTEDGE, WindowClassName, WindowTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WindowWidth, WindowHeight, NULL, NULL, hInstance, NULL);

	if (hwnd == NULL)
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	ImGui::CreateContext();
	ImGuiIO& io{ ImGui::GetIO() };
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();
	
    ImGui_ImplWin32_Init(hwnd);

	MSG msg;
	msg.message = WM_NULL;
	msg.wParam = -1;
	{
		std::shared_ptr<IGraphics> graphics = std::make_shared<DX11Graphics>(hwnd);
		std::shared_ptr<IInput> input = std::make_shared<DirectXInput>();
		std::unique_ptr<IApplication> application = GetApplication(graphics, input);
		std::unique_ptr<Editor> editor = std::make_unique<Editor>(*graphics);

		if (graphics && graphics->IsValid() && application)
		{
			application->Load();

			while (msg.message != WM_QUIT && application->IsValid())
			{
				if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

				if (doResize)
				{
					RECT dimensions;
					GetClientRect(hwnd, &dimensions);
					uint32_t windowWidth = dimensions.right - dimensions.left;
					uint32_t windowHeight = dimensions.bottom - dimensions.top;

					graphics->SetScreenSize(windowWidth, windowHeight);
					doResize = false;
				}

				graphics->BeginUpdate();

				ImGui_ImplWin32_NewFrame();
				ImGui::NewFrame();

				input->Update();
				application->Update();

				graphics->Update();

				editor->Update();

				ImGui::Render();
				graphics->EndUpdate();
			}


			application->Cleanup();
		}
	}

	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	return static_cast<int>(msg.wParam);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool minimized = false;
bool inSizeMove = false;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
		{
			// DEVNOTE: Application is minimized, should probably suspend.
			if (!minimized)
				minimized = true;
		}
		else if (minimized)
		{
			minimized = false;
		}
		else if (inSizeMove)
		{
			doResize = true;
		}
		else if (!inSizeMove)
		{
			doResize = true;
		}
		break;
	case WM_ENTERSIZEMOVE:
		inSizeMove = true;
		break;
	case WM_EXITSIZEMOVE:
		inSizeMove = false;
		break;
	case WM_GETMINMAXINFO:
	{
		auto info = reinterpret_cast<MINMAXINFO*>(lParam);
		info->ptMinTrackSize.x = 320;
		info->ptMinTrackSize.y = 200;
	}
	break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}
