#pragma once
#include "stdafx.h"

namespace function {
	namespace utils {
		extern HANDLE hProcess;
		extern DWORD baseAddress;
		extern HWND hWnd;
		bool isFocusOnAVA();
		bool isFile(const std::string& name);
	}
}