#include "stdafx.h"
#include "Memory.h"

DWORD Memory::getModuleBase(const wchar_t* ModuleName, DWORD procID)
{
	MODULEENTRY32 ModuleEntry = { 0 };
	HANDLE SnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procID);
	if (!SnapShot) return NULL;
	ModuleEntry.dwSize = sizeof(ModuleEntry);
	if (!Module32First(SnapShot, &ModuleEntry)) return NULL;
	do{
		if (!wcscmp(ModuleEntry.szModule, ModuleName)){
			CloseHandle(SnapShot);
			return (DWORD)ModuleEntry.modBaseAddr;
		}
	} while (Module32Next(SnapShot, &ModuleEntry));
	CloseHandle(SnapShot);
	return NULL;
}

DWORD Memory::readPointer(HANDLE hProcess, DWORD baseAddress, vector<DWORD> offsets) {
	DWORD p = baseAddress;
	for (vector<DWORD>::iterator it = offsets.begin(); it != offsets.end(); ++it) {
		p = read_remote_pointer(hProcess, p);
		if (!p)	return 0;
		p += *it;
	}
	return p;
}

//private
DWORD Memory::read_remote_pointer(HANDLE process, DWORD address) {
	char buffer[sizeof(DWORD)];
	SIZE_T bytes_read;
	if (!ReadProcessMemory(process, (const void *)address, buffer, sizeof(buffer), &bytes_read) || bytes_read != sizeof(buffer)) return 0;
	return *(DWORD *)buffer;
}

