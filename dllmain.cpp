// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include "Memory.h"
#include "logConsole.h"
#include "d3dmenu.h"
#include "HackFunction.h"
#include "hook_function.h"

using namespace std;

//D3D
//=====================================================================================

DWORD WINAPI VirtualMethodTableRepatchingLoopToCounterExtensionRepatching(LPVOID);

//=====================================================================================

class Memory Memory;
class d3dMenu d3dmenu;
class logConsole console;

//process details
HANDLE hProcess;
DWORD baseAddress;

bool menu = false;

bool func_changename = false,
	 func_wallhack = false,
	 func_QQMacro = false;

BYTE orignal_name[72] = { NULL };


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
	
	console.logMessage("Debug Console Started...", 0);
	console.logMessage("Debug Console Started...", 1);
	console.logMessage("Debug Console Started...", 2);
	console.logMessage("Debug Console Started...", 3);
	while (1)
	{
		Sleep(100);
		if (isFocusOnAVA()) {

			if ((GetAsyncKeyState(VK_HOME) & 0x1)) {
				menu = !menu;
			}
			if ((GetAsyncKeyState(VK_F4) & 0x1) && menu) {
				ToggleWallHack();
			}
			if ((GetAsyncKeyState(VK_F5) & 0x1) && menu) {
				ToggleChangeName();
			}
			if ((GetAsyncKeyState(VK_F6) & 0x1) && menu) {
				ToggleQQMacro();
			}
			
			DoQQMacro();
		}



		hookD3Dfunction();
	}

	return 1;
}
