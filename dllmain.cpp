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
CreateDevice_Prototype         CreateDevice_Pointer = NULL;
Reset_Prototype                Reset_Pointer = NULL;
EndScene_Prototype             EndScene_Pointer = NULL;
DrawIndexedPrimitive_Prototype DrawIndexedPrimitive_Pointer = NULL;
CreateQuery_Prototype          CreateQuery_Pointer = NULL;

DWORD WINAPI VirtualMethodTableRepatchingLoopToCounterExtensionRepatching(LPVOID);
PDWORD Direct3D_VMTable = NULL;
//=====================================================================================

class Memory Memory;
class d3dMenu d3dmenu;
class logConsole console;

bool menu = true;


bool func_changename = false,
	 func_wallhack = false,
	 func_QQMacro = false;



BYTE orignal_name[72] = { NULL };

LPD3DXFONT g_font_default = NULL;
LPDIRECT3DTEXTURE9 texture_Red = NULL, texture_Black = NULL;
IDirect3DPixelShader9 *Front, *Back;


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


//fetch information
HANDLE hProcess;
DWORD baseAddress;

DWORD WINAPI VirtualMethodTableRepatchingLoopToCounterExtensionRepatching(LPVOID Param)
{
	UNREFERENCED_PARAMETER(Param);

	hProcess = GetCurrentProcess();
	baseAddress = Memory.getModuleBase(L"AVA.exe", GetCurrentProcessId());

	//start logConsole
	console.startConsole();
	console.logMessage("[Message] Debug Console Started...", false);

	while (1)
	{
		Sleep(100);

		if (GetForegroundWindow() == FindWindowA("LaunchUnrealUWindowsClient", "Alliance of Valiant Arms")) {
			
			//if ((GetAsyncKeyState(VK_HOME) & 0x1)) 
				//menu = !menu;
			if ((GetAsyncKeyState(VK_F4) & 0x1) && menu)
				ToggleWallHack();
			if ((GetAsyncKeyState(VK_F5) & 0x1) && menu)
				ToggleChangeName();
			if ((GetAsyncKeyState(VK_F6) & 0x1) && menu)
				ToggleQQMacro();

			DoQQMacro();
		}



		*(PDWORD)&Direct3D_VMTable[16] = (DWORD)Reset_Detour;
		*(PDWORD)&Direct3D_VMTable[42] = (DWORD)EndScene_Detour;
		*(PDWORD)&Direct3D_VMTable[82] = (DWORD)DrawIndexedPrimitive_Detour;
		*(PDWORD)&Direct3D_VMTable[118] = (DWORD)CreateQuery_Detour;
	}

	return 1;
}
