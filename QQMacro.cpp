#include "QQMacro.h"

#include "utils.h"


namespace function {
	namespace QQMacro {

		void doQQMacro() {
			//LoadKeyboardLayout((LPCWSTR)"00000409", KLF_ACTIVATE);
			if (utils::isFocusOnAVA()) keybd_event(0x51, 0, 0, 0);
			Sleep(150);
			if (utils::isFocusOnAVA()) keybd_event(0x51, 0, KEYEVENTF_KEYUP, 0);
			Sleep(100);
			if (utils::isFocusOnAVA()) keybd_event(0x51, 0, 0, 0);
			Sleep(25);
			if (utils::isFocusOnAVA()) keybd_event(0x51, 0, KEYEVENTF_KEYUP, 0);
			Sleep(625);

		}
	}
}