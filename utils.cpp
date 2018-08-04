#include "utils.h"

namespace function {
	namespace utils {
		HANDLE hProcess = 0;
		DWORD baseAddress = 0;
		HWND hWnd = 0;
		bool isFocusOnAVA() {
			return (GetForegroundWindow() == FindWindowA("LaunchUnrealUWindowsClient", "Alliance of Valiant Arms"));
		}

	}
}