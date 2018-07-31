#include "newMenu.h"

//IMGUI Library
#include "lib/imgui/imgui.h"
#include "lib/imgui/imgui_impl_dx9.h"

bool isInitMENU = false;
WNDPROC oriWndProc = NULL;

IMGUI_API LRESULT ImGui_ImplDX9_WndProcHandler(HWND, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ImGuiIO& io = ImGui::GetIO();
	switch (msg)
	{
	case WM_LBUTTONDOWN:
		io.MouseDown[0] = true;
		return true;
	case WM_LBUTTONUP:
		io.MouseDown[0] = false;
		return true;
	case WM_RBUTTONDOWN:
		io.MouseDown[1] = true;
		return true;
	case WM_RBUTTONUP:
		io.MouseDown[1] = false;
		return true;
	case WM_MBUTTONDOWN:
		io.MouseDown[2] = true;
		return true;
	case WM_MBUTTONUP:
		io.MouseDown[2] = false;
		return true;
	case WM_MOUSEWHEEL:
		io.MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
		return true;
	case WM_MOUSEMOVE:
		io.MousePos.x = (signed short)(lParam);
		io.MousePos.y = (signed short)(lParam >> 16);
		return true;
	case WM_KEYDOWN:
		if (wParam < 256)
			io.KeysDown[wParam] = 1;
		return true;
	case WM_KEYUP:
		if (wParam < 256)
			io.KeysDown[wParam] = 0;
		return true;
	case WM_CHAR:
		// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
		if (wParam > 0 && wParam < 0x10000)
			io.AddInputCharacter((unsigned short)wParam);
		return true;
	}
	return 0;
}


LRESULT CALLBACK hWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (function::menu::isMENU && ImGui_ImplDX9_WndProcHandler(hwnd, uMsg, wParam, lParam)) return 1;
	return CallWindowProc(oriWndProc, hwnd, uMsg, wParam, lParam);
}

namespace function {
	namespace menu {
		namespace item {
			bool checkbox_wallhack = false;
			bool checkbox_QQMacro = false;
			bool checkbox_ChangeName = false;
		}

		bool isMENU = false;

		void initMenu(IDirect3DDevice9 *pDevice) {
			if (!isInitMENU) {
				// Setup Dear ImGui binding
				ImGui::CreateContext();
				ImGuiIO& io = ImGui::GetIO();
				D3DDEVICE_CREATION_PARAMETERS d3dcp{ 0 };
				pDevice->GetCreationParameters(&d3dcp);
				RECT rect;
				GetWindowRect(d3dcp.hFocusWindow, &rect);
				oriWndProc = (WNDPROC)SetWindowLongPtr(d3dcp.hFocusWindow, GWL_WNDPROC, (LONG_PTR)hWndProc);
				io.DisplaySize.x = (float)(rect.right - rect.left);
				io.DisplaySize.y = (float)(rect.bottom - rect.top);
				io.Fonts->AddFontDefault();
				//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
				ImGui_ImplDX9_Init(pDevice);
				ImGui::StyleColorsDark();
				isInitMENU = true;
			}
		}

		void MenuRender()
		{
			if (!isMENU) return;

			ImGui::GetIO().MouseDrawCursor = isMENU;

			ImGui_ImplDX9_NewFrame();
			ImGui::NewFrame();

			if (ImGui::Begin("AVA Hack 2018",0, ImVec2(250,450), 0.85f, ImGuiWindowFlags_NoResize)) {
				ImGui::Checkbox("WallHack", &item::checkbox_wallhack);
				ImGui::Checkbox("QQMacro", &item::checkbox_QQMacro);
				ImGui::Checkbox("ChangeName", &item::checkbox_ChangeName);
				ImGui::End();
			}

			ImGui::EndFrame();
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());


		}

	}
}