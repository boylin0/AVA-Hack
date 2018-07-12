#pragma once

enum loginfo_type {
	info = 0 ,
	error = 1 ,
	warning = 2 ,
	debug = 3
};

class logConsole {
public:
	void startConsole();
	void logMessage(char *msg, DWORD type, bool p_time = true, bool p_newline = true);
};