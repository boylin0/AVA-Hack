#include "stdafx.h"
#include "HackFunction.h"
#include "QQMacro.h"
#include "newMenu.h"
#include "utils.h"

#define infoBaseAddress     0x0242F044    //00 00 EF 28 18 57 C2 20 00 00 00 00 00 00
#define weaponBaseAddress   0x0244BFEC

using namespace function;

class Memory memory;

BYTE orignal_name[22] = { NULL };

std::wstring utf8_to_utf16(const std::string& utf8)
{
	std::vector<unsigned long> unicode;
	size_t i = 0;
	while (i < utf8.size())
	{
		unsigned long uni;
		size_t todo;
		bool error = false;
		unsigned char ch = utf8[i++];
		if (ch <= 0x7F)
		{
			uni = ch;
			todo = 0;
		}
		else if (ch <= 0xBF)
		{
			throw std::logic_error("not a UTF-8 string");
		}
		else if (ch <= 0xDF)
		{
			uni = ch & 0x1F;
			todo = 1;
		}
		else if (ch <= 0xEF)
		{
			uni = ch & 0x0F;
			todo = 2;
		}
		else if (ch <= 0xF7)
		{
			uni = ch & 0x07;
			todo = 3;
		}
		else
		{
			throw std::logic_error("not a UTF-8 string");
		}
		for (size_t j = 0; j < todo; ++j)
		{
			if (i == utf8.size())
				throw std::logic_error("not a UTF-8 string");
			unsigned char ch = utf8[i++];
			if (ch < 0x80 || ch > 0xBF)
				throw std::logic_error("not a UTF-8 string");
			uni <<= 6;
			uni += ch & 0x3F;
		}
		if (uni >= 0xD800 && uni <= 0xDFFF)
			throw std::logic_error("not a UTF-8 string");
		if (uni > 0x10FFFF)
			throw std::logic_error("not a UTF-8 string");
		unicode.push_back(uni);
	}
	std::wstring utf16;
	for (size_t i = 0; i < unicode.size(); ++i)
	{
		unsigned long uni = unicode[i];
		if (uni <= 0xFFFF)
		{
			utf16 += (wchar_t)uni;
		}
		else
		{
			uni -= 0x10000;
			utf16 += (wchar_t)((uni >> 10) + 0xD800);
			utf16 += (wchar_t)((uni & 0x3FF) + 0xDC00);
		}
	}
	return utf16;
}



void DoChangeRank() {
	if (menu::item::checkbox_ChangeRank) {
		vector<DWORD> offsets = { 0x90, 0x3E };
		DWORD p = memory.readPointer(utils::hProcess, utils::baseAddress + infoBaseAddress, offsets);
		WriteProcessMemory(utils::hProcess, (LPVOID)p, &menu::item::slider_Rank, sizeof(menu::item::slider_Rank), 0);
	}
}

void DoChangeName() {
	if (menu::item::checkbox_ChangeName) {
		vector<DWORD> offsets = { 0x90, 0x28 };
		//0242F044
		DWORD p = memory.readPointer(utils::hProcess, utils::baseAddress + infoBaseAddress, offsets);
		if (p == 0) {
			//error
			console::message("ChangeName failed", 1);
		}
		else {
			if (orignal_name[0] == 0x0) ReadProcessMemory(utils::hProcess, (LPCVOID)p, &orignal_name, sizeof(orignal_name), 0);
			//41 00 48 00 51 00 20 00 41 00 66 00 74 00 65 00 52 00
			//BYTE name[] = { 0x61, 0x00, 0x68, 0x00, 0x71, 0x00, 0x5F, 0x00, 0x41, 0x00, 0x66, 0x00, 0x74, 0x00, 0x65, 0x00, 0x52, 0x00 ,0x00 ,0x00, 0x00 ,0x00 };
			wstring buf = utf8_to_utf16(menu::item::input_ChangeName);
			const BYTE* w = reinterpret_cast<const BYTE*>(buf.c_str());
			BYTE name[22] = { 0x0 };
			for (int i = 0; i < sizeof(name) / sizeof(BYTE); ++i) {
				name[i] = w[i];
			}
			WriteProcessMemory(utils::hProcess, (LPVOID)p, &name, sizeof(name), 0);
		}
	} else if(orignal_name[0] != 0x0) {
		vector<DWORD> offsets = { 0x90, 0x28 };
		DWORD p = memory.readPointer(utils::hProcess, utils::baseAddress + infoBaseAddress, offsets);
		if (p == 0) {
			//error
			console::message("ChangeName failed", 1);
		}
		else {
			WriteProcessMemory(utils::hProcess, (LPVOID)p, &orignal_name, sizeof(orignal_name), 0);
		}
	}
}


void DoQQMacro() {
	if (menu::item::checkbox_QQMacro && (GetAsyncKeyState(VK_LBUTTON) & 0x1) && utils::isFocusOnAVA && !menu::isMENU) {
		function::QQMacro::doQQMacro();
	}
}

void DoUnlimitAmmo() {
	if (menu::item::checkbox_unlimitAmmo) {
		vector<DWORD> offsets = { 0x428, 0x284, 0x7A2 };
		DWORD p = memory.readPointer(utils::hProcess, utils::baseAddress + weaponBaseAddress, offsets);
		BYTE myval[1] = { 0x1 };
		WriteProcessMemory(utils::hProcess, (LPVOID)p, &myval, sizeof(myval), 0);
		offsets = { 0x428, 0x284, 0x338 };
		p = memory.readPointer(utils::hProcess, utils::baseAddress + weaponBaseAddress, offsets);
		myval[1] = { 0x0 };
		WriteProcessMemory(utils::hProcess, (LPVOID)p, &myval, sizeof(myval), 0);
	}
}

void DoNoSpread() {
	if (menu::item::checkbox_noSpread) {
		vector<DWORD> offsets = { 0x428, 0x284, 0x8BC };
		DWORD p = memory.readPointer(utils::hProcess, utils::baseAddress + weaponBaseAddress, offsets);
		int myval = 0;
		WriteProcessMemory(utils::hProcess, (LPVOID)p, &myval, sizeof(myval), 0);
	}
}

void DoMemoryHack() {
	DoQQMacro();
	DoChangeRank();
	DoChangeName();
	DoUnlimitAmmo();
	DoNoSpread();
}