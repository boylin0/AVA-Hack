#pragma once
#include "stdafx.h"


class Memory{
public:
	DWORD getModuleBase(const wchar_t* ModuleName, DWORD procID);
	DWORD readPointer(HANDLE hProcess, DWORD baseAddress, DWORD offsets[]);
private:
	DWORD read_remote_pointer(HANDLE process, DWORD address);
};