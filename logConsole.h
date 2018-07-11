#pragma once

class logConsole {
public:
	void startConsole();
	void logMessage(char *msg, bool time = true, bool newline = true);
};