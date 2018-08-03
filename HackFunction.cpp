#include "stdafx.h"
#include "HackFunction.h"
#include "QQMacro.h"
#include "newMenu.h"
#include "utils.h"

using namespace function;

class Memory Memory;
HANDLE hProcess = NULL;
DWORD baseAddress = NULL;
BYTE orignal_name[22] = { NULL };

//00 00 EF 28 18 57 C2 20 00 00 00 00 00 00
const DWORD infoBaseAddress = 0x0242F044;

void DoChangeRank() {
	if (menu::item::checkbox_ChangeRank) {
		DWORD offsets[] = { 0x90, 0x3E };
		DWORD p = Memory.readPointer(hProcess, baseAddress + infoBaseAddress, offsets);
		WriteProcessMemory(hProcess, (LPVOID)p, &menu::item::slider_Rank, sizeof(menu::item::slider_Rank), 0);
	}
}

void DoChangeName() {
	if (menu::item::checkbox_ChangeName) {
		DWORD offsets[] = { 0x90, 0x28 };
		//0242F044
		DWORD p = Memory.readPointer(hProcess, baseAddress + infoBaseAddress, offsets);
		if (p == 0) {
			//error
			console::message("ChangeName failed", 1);
		}
		else {
			if (orignal_name[0] == 0x0) ReadProcessMemory(hProcess, (LPCVOID)p, &orignal_name, sizeof(orignal_name), 0);
			//41 00 48 00 51 00 20 00 41 00 66 00 74 00 65 00 52 00
			BYTE name[] = { 0x61, 0x00, 0x68, 0x00, 0x71, 0x00, 0x5F, 0x00, 0x41, 0x00, 0x66, 0x00, 0x74, 0x00, 0x65, 0x00, 0x52, 0x00 ,0x00 ,0x00, 0x00 ,0x00 };
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


void DoQQMacro() {
	if (menu::item::checkbox_QQMacro && (GetAsyncKeyState(VK_LBUTTON) & 0x1) && utils::isFocusOnAVA) {
		function::QQMacro::doQQMacro();
	}
}