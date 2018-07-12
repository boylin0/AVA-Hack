#include "stdafx.h"
#include "logConsole.h"


using namespace std;
HANDLE hConsole ;

void logConsole::startConsole() {
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
}

void logConsole::logMessage(char *msg, DWORD type, bool p_time, bool p_newline) {
	if (p_time) {
		time_t now = time(0);
		tm *ltm = localtime(&now);
		cout <<  "["
			 << setfill('0') << setw(2) << ltm->tm_hour << ":"
		 	 << setfill('0') << setw(2) << ltm->tm_min << ":" 
		 	 << setfill('0') << setw(2) << ltm->tm_sec << "]";
	}
	switch (type) {
	case 0:
		SetConsoleTextAttribute(hConsole, 143);
		cout << "[INFO]";
		break;
	case 1:
		SetConsoleTextAttribute(hConsole, 79);
		cout << "[ERROR]";
		break;
	case 2:
		SetConsoleTextAttribute(hConsole, 111);
		cout << "[WARNING]";
		break;
	case 3:
		SetConsoleTextAttribute(hConsole, 63);
		cout << "[DEBUG]";
		break;
	}
	SetConsoleTextAttribute(hConsole, 15);
	cout << " " << msg;
	if (p_newline) cout << std::endl;
}