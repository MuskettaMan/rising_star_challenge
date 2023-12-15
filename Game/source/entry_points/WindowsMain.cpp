// The main Windows / DirectX Graphics / XInput entry point for Star Applications

#define WIN32_LEAN_AND_MEAN

#include "pch.h"
#include "Engine/Implementation/DirectX11/DirectX11Graphics.h"
#include "Engine/Implementation/XInput/DirectXInput.h"
#include "Engine/IRenderable.h"
#include "Engine/ITexture.h"
#include "Engine/IShader.h"
#include "Engine/IApplication.h"

const char WindowClassName[] = "Star";
const char WindowTitle[] = "Search for a Star 2024";
const int WindowWidth = 1920;
const int WindowHeight = 1080;

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
		std::shared_ptr<IGraphics> Graphics = std::make_shared<DirectX11Graphics>(hwnd);
		std::shared_ptr<IInput> Input = std::make_shared<DirectXInput>();
		std::unique_ptr<IApplication> Application = GetApplication(Graphics, Input);

		if (Graphics && Graphics->IsValid() && Application)
		{
			Application->Load();

			while (msg.message != WM_QUIT && Application->IsValid())
			{
				if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				Graphics->BeginUpdate();

				ImGui_ImplWin32_NewFrame();
				ImGui::NewFrame();

				Input->Update();
				Application->Update();

				Graphics->Update();

				ImGui::Render();
				Graphics->EndUpdate();
			}


			Application->Cleanup();
		}
	}

	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	return static_cast<int>(msg.wParam);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

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
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}
