#include <time.h>
#include "QQMacro.h"
#include "utils.h"



void PressKey(BYTE vKey) {
	keybd_event(vKey, 0, 0, 0);
	Sleep(rand() % 25 + 1);
	keybd_event(vKey, 0, KEYEVENTF_KEYUP, 0);
	Sleep(rand() % 25 + 1);
}

namespace function {
	namespace QQMacro {

		void doQQMacro() {
			Sleep(rand() % 30 + 1);
			if (utils::isFocusOnAVA()) PressKey(0x33);
			Sleep(rand() % 30 + 1);
			if (utils::isFocusOnAVA()) PressKey(0x31);
			Sleep(550);
			//LoadKeyboardLayout((LPCWSTR)"00000409", KLF_ACTIVATE);

		}
	}
}