#include "stdafx.h"
#include "HackFunction.h"

void ToggleWallHack() {
	func_wallhack = !func_wallhack;
	if (func_wallhack)
	{
		d3dmenu.moditem(2, "[F4] WallHack", D3DCOLOR_ARGB(255, 000, 255, 000)); cout << "[INFO] WallHack on\n";
	} //wallhack on
	else { d3dmenu.moditem(2, "[F4] WallHack", D3DCOLOR_ARGB(255, 255, 000, 000)); cout << "[INFO] WallHack off\n"; } //wallhack off
}

void ToggleChangeName() {
	func_changename = !func_changename;
	if (func_changename) {
		//changename on
		d3dmenu.moditem(3, "[F5] ChangeName", D3DCOLOR_ARGB(255, 000, 255, 000)); cout << "[INFO] ChangeName on\n";
		DWORD offsets[] = { 0x90, 0x28 };
		DWORD p = Memory.readPointer(hProcess, baseAddress + 0x242BFE4, offsets);
		if (p == 0) {
			console.logMessage("[Error] ChangeName failed");
		}
		else {
			if (orignal_name[0] == 0x0) ReadProcessMemory(hProcess, (LPCVOID)p, &orignal_name, sizeof(orignal_name), 0);
			BYTE name[] = { 0x5B ,0x00, 0x55, 0x00 ,0x6E ,0x00 ,0x6B ,0x00 ,0x6E ,0x00 ,0x6F ,0x00 ,0x77 ,0x00 ,0x6E ,0x00 ,0x5D };
			WriteProcessMemory(hProcess, (LPVOID)p, &name, sizeof(name), 0);
		}
	}
	else {
		//changename off
		d3dmenu.moditem(3, "[F5] ChangeName", D3DCOLOR_ARGB(255, 255, 000, 000)); cout << "[INFO] ChangeName off\n";
		DWORD offsets[] = { 0x90, 0x28 };
		DWORD p = Memory.readPointer(hProcess, baseAddress + 0x242BFE4, offsets);
		if (p == 0) {
			console.logMessage("[Error] ChangeName failed");
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
		d3dmenu.moditem(4, "[F6] QQMacro", D3DCOLOR_ARGB(255, 000, 255, 000)); cout << "[INFO] QQMacro on\n";
	}
	else
	{
		d3dmenu.moditem(4, "[F6] QQMacro", D3DCOLOR_ARGB(255, 255, 000, 000)); cout << "[INFO] QQMacro off\n";
	}
}

void DoQQMacro() {
	if (func_QQMacro && (GetAsyncKeyState(VK_LBUTTON) & 0x1)) {
		keybd_event(0x51, 0, 0, 0);
		Sleep(75);
		keybd_event(0x51, 0, KEYEVENTF_KEYUP, 0);
		Sleep(100);
		keybd_event(0x51, 0, 0, 0);
		Sleep(25);
		keybd_event(0x51, 0, KEYEVENTF_KEYUP, 0);
		Sleep(25);
		Sleep(550);
	}
}