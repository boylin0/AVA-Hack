#include "stdafx.h"
#include "logConsole.h"

void logConsole::startConsole() {
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
}

void logConsole::logMessage(char *msg, bool time, bool newline ) {
	if (time) { std::cout << "[time]"; }
	std::cout << msg;
	if (newline) std::cout << std::endl;
}