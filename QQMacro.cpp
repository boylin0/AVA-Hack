#include <time.h>
#include "QQMacro.h"
#include "utils.h"

void PressQ() {
	keybd_event(0x51, 0, 0, 0);
	Sleep(rand() % 25 + 1);
	keybd_event(0x51, 0, KEYEVENTF_KEYUP, 0);
	Sleep(rand() % 25 + 1);
}

namespace function {
	namespace QQMacro {

		void doQQMacro() {
			Sleep(rand() % 80 + 1);
			if (utils::isFocusOnAVA()) PressQ();
			Sleep(rand() % 80 + 1);
			if (utils::isFocusOnAVA()) PressQ();
			Sleep(500);
			//LoadKeyboardLayout((LPCWSTR)"00000409", KLF_ACTIVATE);

		}
	}
}