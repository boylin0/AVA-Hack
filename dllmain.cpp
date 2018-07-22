// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include "Memory.h"
#include "logConsole.h"
#include "d3dmenu.h"
#include "HackFunction.h"
#include "hook_function.h"

//IMGUI Library
#include "lib/imgui/imgui.h"
#include "lib/imgui/imgui_impl_dx9.h"

class Memory Memory;
class d3dMenu d3dmenu;
class logConsole console;

bool func_changename,
	 func_wallhack,
	 func_QQMacro;

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		if (Direct3DCreate9_VMTable() == D3D_OK) return TRUE;
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

DWORD WINAPI VirtualMethodTableRepatchingLoopToCounterExtensionRepatching(LPVOID Param)
{
	UNREFERENCED_PARAMETER(Param);

	hProcess = GetCurrentProcess();
	baseAddress = Memory.getModuleBase(L"AVA.exe", GetCurrentProcessId());

	//start logConsole
	console.logMessage("Game Window Started", 0);

	while (1)
	{
		Sleep(100);
		if (isFocusOnAVA()) {

			if ((GetAsyncKeyState(VK_HOME) & 0x1)) {
				menu = !menu;
				console.logMessage("Menu:", 3, true, true, false);
				console.logMessage( (menu == 1 ? "On":"Off") , 3, false, false, true);
			}
			if ((GetAsyncKeyState(VK_F4) & 0x1) && menu) {
				ToggleWallHack();
				console.logMessage("function_wallhack:", 3, true, true, false);
				console.logMessage( (func_wallhack == 1 ? "On" : "Off") , 3, false, false, true);
			}
			if ((GetAsyncKeyState(VK_F5) & 0x1) && menu) {
				ToggleChangeName();
				console.logMessage("function_changename:", 3, true, true, false);
				console.logMessage( (func_changename == 1 ? "On" : "Off") , 3, false, false, true);
			}
			if ((GetAsyncKeyState(VK_F6) & 0x1) && menu) {
				ToggleQQMacro();
				console.logMessage("function_QQMacro:", 3, true, true, false);
				console.logMessage( (func_QQMacro == 1 ? "On" : "Off") , 3, false, false, true);
			}
			
			DoQQMacro();
		}



		hookD3Dfunction();
	}

	return 1;
}
