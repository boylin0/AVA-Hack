#include "newMenu.h"
#include "HackFunction.h"

//IMGUI Library
#include "lib/imgui/imgui.h"
#include "lib/imgui/imgui_impl_dx9.h"
#include "lib/imgui/imgui_impl_win32.h"

bool isInitMENU = false;
WNDPROC oriWndProc = NULL;


extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
//extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK hWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (function::menu::isMENU && ImGui_ImplDX9_WndProcHandler(hwnd, uMsg, wParam, lParam)) return true;
	return CallWindowProc(oriWndProc, hwnd, uMsg, wParam, lParam);
}

namespace function {
	namespace menu {
		namespace item {
			bool  checkbox_wallhack = false;
			bool  checkbox_aimbot = false;
			bool  checkbox_QQMacro = false;
			bool  checkbox_ChangeName = false;
			bool  checkbox_ChangeRank = false;
			bool  checkbox_debugMode = false;
			bool  checkbox_unlimitAmmo = false;
			bool  checkbox_noSpread = false;
			int   slider_Rank = 65;
			float slider_aimheight = 0;
			float slider_aimspeed = 4;
			char  input_ChangeName[30];
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
				io.Fonts->AddFontFromFileTTF("..\\avaGame\\Fonts\\TPE\\AVAFont.TTF", 14.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
				//io.Fonts->AddFontDefault();
				//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
				ImGui_ImplWin32_Init(d3dcp.hFocusWindow);
				ImGui_ImplDX9_Init(pDevice);
				io.ImeWindowHandle = d3dcp.hFocusWindow;
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

			if (ImGui::Begin("AVA Hack 2018",0, ImVec2(400.0f, 450.0f), 0.75f, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
				ImGui::Checkbox(u8"透視", &item::checkbox_wallhack);
				ImGui::Separator();

				ImGui::Checkbox(u8"自瞄", &item::checkbox_aimbot);
				ImGui::SliderFloat(u8"高度", &item::slider_aimheight, -100.0f, 100.0f, "%.0f", 1.0f);
				ImGui::SameLine();
				if(ImGui::Button(u8"重設")){
					item::slider_aimheight = 0;
				}

				ImGui::SliderFloat(u8"平滑移動", &item::slider_aimspeed, 1.0f, 8.0f, "%.2f");
				ImGui::SameLine();
				if (ImGui::Button(u8"重設")) item::slider_aimspeed = 4.0f;
				if (ImGui::Button(u8"－")) item::slider_aimspeed -= 0.01f; 
				ImGui::SameLine();
				if (ImGui::Button(u8"＋")) item::slider_aimspeed += 0.01f;
				ImGui::Separator();

				ImGui::Checkbox(u8"無限子彈", &item::checkbox_unlimitAmmo);
				ImGui::Separator();

				ImGui::Checkbox(u8"防止子彈飄移", &item::checkbox_noSpread);
				ImGui::Separator();

				ImGui::Checkbox(u8"自動切槍[F4]", &item::checkbox_QQMacro);
				ImGui::Separator();

				ImGui::Checkbox(u8"修改名稱", &item::checkbox_ChangeName);
				ImGui::InputText("", item::input_ChangeName, IM_ARRAYSIZE(item::input_ChangeName));
				ImGui::Separator();

				ImGui::Checkbox(u8"修改等級", &item::checkbox_ChangeRank);
				ImGui::SliderInt(u8"等級", &item::slider_Rank, 0.0f, 65.0f, "%.0f");
				ImGui::Separator();

				ImGui::Checkbox(u8"DEBUG", &item::checkbox_debugMode);
				ImGui::Separator();
				ImGui::End();
			}

			ImGui::EndFrame();
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());


		}

	}
}