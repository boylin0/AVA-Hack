// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include "Memory.h"
#include "HackFunction.h"
#include "hook_function.h"
#include "newMenu.h"
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

	hProcess = GetCurrentProcess();
	baseAddress = Memory.getModuleBase(L"AVA.exe", GetCurrentProcessId());

	//start logConsole
	console::message("Game Window Started", 0);

	while (1)
	{
		Sleep(100);
		if (isFocusOnAVA()) {
			if (GetAsyncKeyState(VK_F4) & 0x1) {
				function::menu::item::checkbox_QQMacro = !function::menu::item::checkbox_QQMacro;
			}
			DoChangeName();
			DoQQMacro();
		}



		hookD3Dfunction();
	}

	return 1;
}
