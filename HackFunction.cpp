#include "stdafx.h"
#include "HackFunction.h"

using namespace function;

bool menu = false;
HANDLE hProcess = NULL;
DWORD baseAddress = NULL;
BYTE orignal_name[22] = { NULL };

void ToggleWallHack() {
	func_wallhack = !func_wallhack;
	if (func_wallhack)
	{
		d3dmenu.moditem(2, "[F4] WallHack", D3DCOLOR_ARGB(255, 000, 255, 000));
	} //wallhack on
	else { d3dmenu.moditem(2, "[F4] WallHack", D3DCOLOR_ARGB(255, 255, 000, 000)); } //wallhack off
}

void ToggleChangeName() {
	func_changename = !func_changename;
	if (func_changename) {
		//changename on
		d3dmenu.moditem(3, "[F5] ChangeName", D3DCOLOR_ARGB(255, 000, 255, 000));
		DWORD offsets[] = { 0x90, 0x28 };
		DWORD p = Memory.readPointer(hProcess, baseAddress + 0x242BFE4, offsets);
		if (p == 0) {
			console::message("ChangeName failed", 1);
		}
		else {
			if (orignal_name[0] == 0x0) ReadProcessMemory(hProcess, (LPCVOID)p, &orignal_name, sizeof(orignal_name), 0);
			BYTE name[] = { 0x54 ,0x00 ,0x50 ,0x00, 0x41, 0x00, 0x5F, 0x00, 0x52, 0x00 ,0x61 ,0x00, 0x70, 0x00, 0x69 ,0x00, 0x64 ,0x00  ,0x00 ,0x00, 0x00 ,0x00 };

			//BYTE name[] = { 0x45, 0x00 ,0x6C ,0x00, 0x74 ,0x00, 0x61, 0x00 ,0x6E, 0x00, 0x00 ,0x00 ,0x00, 0x00 ,0x00 ,0x00 ,0x00, 0x00, 0x00 ,0x00, 0x00 ,0x00 };
			WriteProcessMemory(hProcess, (LPVOID)p, &name, sizeof(name), 0);
		}
	}
	else {
		//changename off
		d3dmenu.moditem(3, "[F5] ChangeName", D3DCOLOR_ARGB(255, 255, 000, 000));
		DWORD offsets[] = { 0x90, 0x28 };
		DWORD p = Memory.readPointer(hProcess, baseAddress + 0x242BFE4, offsets);
		if (p == 0) {
			console::message("ChangeName failed", 1);
		}
		else {
			WriteProcessMemory(hProcess, (LPVOID)p, &orignal_name, sizeof(orignal_name), 0);
		}
	}
}

void ToggleQQMacro() {
	func_QQMacro = !func_QQMacro;
	if (func_QQMacro)
	{
		d3dmenu.moditem(4, "[F6] QQMacro", D3DCOLOR_ARGB(255, 000, 255, 000)); 
	}
	else
	{
		d3dmenu.moditem(4, "[F6] QQMacro", D3DCOLOR_ARGB(255, 255, 000, 000));
	}
}

bool isFocusOnAVA() {
	return (GetForegroundWindow() == FindWindowA("LaunchUnrealUWindowsClient", "Alliance of Valiant Arms"));
}

void DoQQMacro() {
	if (func_QQMacro && (GetAsyncKeyState(VK_LBUTTON) & 0x1)) {
		LoadKeyboardLayout((LPCWSTR)"00000409", KLF_ACTIVATE);
		if(isFocusOnAVA()) keybd_event(0x51, 0, 0, 0);
		Sleep(100);
		if (isFocusOnAVA()) keybd_event(0x51, 0, KEYEVENTF_KEYUP, 0);
		Sleep(100);
		if (isFocusOnAVA()) keybd_event(0x51, 0, 0, 0);
		Sleep(25);
		if (isFocusOnAVA()) keybd_event(0x51, 0, KEYEVENTF_KEYUP, 0);
		Sleep(625);
	}
}