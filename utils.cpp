#include "utils.h"

namespace function {
	namespace utils {
		bool isFocusOnAVA() {
			return (GetForegroundWindow() == FindWindowA("LaunchUnrealUWindowsClient", "Alliance of Valiant Arms"));
		}
	}
}