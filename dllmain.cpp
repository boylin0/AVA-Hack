// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include "Memory.h"
#include "HackFunction.h"
#include "hook_function.h"
#include "newMenu.h"
#include "utils.h"
//IMGUI Library
#include "lib/imgui/imgui.h"
#include "lib/imgui/imgui_impl_dx9.h"

using namespace function;


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

	//fetch process information for memory hacking
	utils::hProcess = GetCurrentProcess();
	utils::baseAddress = memory.getModuleBase(L"AVA.exe", GetCurrentProcessId());
	utils::hWnd = FindWindowA("LaunchUnrealUWindowsClient", "Alliance of Valiant Arms");

	//start logConsole
	console::message("Game Window Started", 0);

	while (1)
	{
		Sleep(1);

		//toggle QQMacro function by hotkey
		if ( (GetAsyncKeyState(VK_F4) & 0x1) && utils::isFocusOnAVA() ) {
			function::menu::item::checkbox_QQMacro = !function::menu::item::checkbox_QQMacro;
		}
			
		//hack function
		DoMemoryHack();

		//hook d3d
		hookD3Dfunction();
	}

	return 1;
}
