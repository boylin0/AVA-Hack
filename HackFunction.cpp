#include "stdafx.h"
#include "HackFunction.h"
#include "QQMacro.h"
#include "newMenu.h"

using namespace function;

class Memory Memory;
HANDLE hProcess = NULL;
DWORD baseAddress = NULL;
BYTE orignal_name[22] = { NULL };

const DWORD infoBaseAddress = 0x0242F044;

void DoChangeName() {
	if (menu::item::checkbox_ChangeName) {
		//changename on
		DWORD offsets[] = { 0x90, 0x28 };
		//0242F044
		DWORD p = Memory.readPointer(hProcess, baseAddress + infoBaseAddress, offsets);
		if (p == 0) {
			//error
			console::message("ChangeName failed", 1);
		}
		else {
			if (orignal_name[0] == 0x0) ReadProcessMemory(hProcess, (LPCVOID)p, &orignal_name, sizeof(orignal_name), 0);
			BYTE name[] = { 0x54 ,0x00 ,0x50 ,0x00, 0x41, 0x00, 0x5F, 0x00, 0x52, 0x00 ,0x61 ,0x00, 0x70, 0x00, 0x69 ,0x00, 0x64 ,0x00  ,0x00 ,0x00, 0x00 ,0x00 };
			WriteProcessMemory(hProcess, (LPVOID)p, &name, sizeof(name), 0);
		}
	} else if(orignal_name[0] != 0x0) {
		DWORD offsets[] = { 0x90, 0x28 };
		DWORD p = Memory.readPointer(hProcess, baseAddress + infoBaseAddress, offsets);
		if (p == 0) {
			//error
			console::message("ChangeName failed", 1);
		}
		else {
			WriteProcessMemory(hProcess, (LPVOID)p, &orignal_name, sizeof(orignal_name), 0);
		}
	}
}


bool isFocusOnAVA() {
	return (GetForegroundWindow() == FindWindowA("LaunchUnrealUWindowsClient", "Alliance of Valiant Arms"));
}

void DoQQMacro() {
	if (menu::item::checkbox_QQMacro && (GetAsyncKeyState(VK_LBUTTON) & 0x1)) {
		function::QQMacro::doQQMacro();
	}
}