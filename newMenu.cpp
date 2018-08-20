#include "newMenu.h"
#include "HackFunction.h"

#include "utils.h"

#include "IniWriter.h"
#include "IniReader.h"

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
			bool  checkbox_UAV = false;
			bool  checkbox_SuperWeapon = false;
			int   slider_Rank = 65;
			float slider_aimheight = 0;
			float slider_aimspeed = 4;
			char  input_ChangeName[30];
		}

		bool isMENU = false;

		void menuValueCheck() {
			if (!(item::slider_aimheight <= 100.0f && item::slider_aimheight >= -100.0f)) item::slider_aimheight = 1.0f;
			if (!(item::slider_aimspeed <= 8.0f && item::slider_aimspeed >= 1.0f)) item::slider_aimspeed = 4.0f;
		}

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

				//load setting
				if (utils::isFile("hack_setting.ini")) {
					CIniReader iniReader(TEXT(".\\hack_setting.ini"));
					item::slider_aimspeed = iniReader.ReadFloat(L"settings", L"aimspeed", L"%.2f", item::slider_aimspeed);
					item::slider_aimheight = abs(iniReader.ReadInteger(L"settings", L"aimheight", item::slider_aimheight));
					menuValueCheck();
				}
				isInitMENU = true;
			}
		}

		void ShowHelpMarker(const char* desc)
		{
			ImGui::TextDisabled("(?)");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
				ImGui::TextUnformatted(desc);
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}
		}

		void MenuRender()
		{
			if (!isMENU) return;

			ImGui::GetIO().MouseDrawCursor = isMENU;
			ImGui::GetIO().WantCaptureKeyboard = isMENU;

			ImGui_ImplDX9_NewFrame();
			ImGui::NewFrame();

			if (ImGui::Begin("AVA Hack 2018", &isMENU, ImVec2(500.0f, 500.0f), 0.75f, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
				int buttonID = 0;
				ImGui::Checkbox(u8"透視", &item::checkbox_wallhack);
				ImGui::Separator();

				ImGui::Checkbox(u8"自瞄", &item::checkbox_aimbot);

				ImGui::SliderFloat(u8"高度", &item::slider_aimheight, -100.0f, 100.0f, "%.0f", 1.0f);
				ImGui::SameLine(); ShowHelpMarker(u8"調整瞄準的高度\n(往下<- ->往上)");
				ImGui::SameLine(ImGui::GetWindowWidth() - 45); ImGui::PushID(buttonID); if (ImGui::Button(u8"重設")) { item::slider_aimheight = 0; } ImGui::PopID(); buttonID++;
				
				ImGui::SliderFloat(u8"瞄準速度", &item::slider_aimspeed, 1.0f, 8.0f, "%.2f");
				ImGui::SameLine(); ShowHelpMarker(u8"滑鼠靈敏度越高時需要調慢; 滑鼠靈敏度越高時需要調快\n(快<- ->慢)");
				ImGui::SameLine(ImGui::GetWindowWidth() - 45);  ImGui::PushID(buttonID); if (ImGui::Button(u8"重設")) item::slider_aimspeed = 4.0f; ImGui::PopID(); buttonID++;
				if (ImGui::ArrowButton("##left", ImGuiDir_Left)) { item::slider_aimspeed -= 0.01f; };  ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x); if (ImGui::ArrowButton("##right", ImGuiDir_Right)) { item::slider_aimspeed += 0.01f; }
				ImGui::Separator();
				
				ImGui::Checkbox(u8"UAV透視", &item::checkbox_UAV);
				ImGui::Separator();

				ImGui::Checkbox(u8"無限子彈", &item::checkbox_unlimitAmmo);
				ImGui::Separator();

				ImGui::Checkbox(u8"穩定槍枝", &item::checkbox_noSpread);
				ImGui::Separator();

				ImGui::Checkbox(u8"超級武器", &item::checkbox_SuperWeapon);
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

				if (ImGui::Button(u8"儲存設定值")) {
					CIniWriter iniWriter(TEXT(".\\hack_setting.ini"));
					iniWriter.WriteFloat(L"settings", L"aimspeed", L"%.2f", item::slider_aimspeed);
					iniWriter.WriteInteger(L"settings", L"aimheight", item::slider_aimheight);
				}

				ImGui::Separator();

				ImGui::End();
			}

			ImGui::EndFrame();
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());


		}

	}
}