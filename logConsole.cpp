#include "stdafx.h"
#include "logConsole.h"


using namespace std;
HANDLE hConsole;

namespace function {
	namespace console {
		void start() {
			AllocConsole();
			freopen("CONOUT$", "w", stdout);
			hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		}

		void message(char *msg, DWORD type, bool showType, bool p_time, bool p_newline) {

			switch (type) {
			case 0:
				SetConsoleTextAttribute(hConsole, 63);
				if (showType) cout << "[INFO]";
				break;
			case 1:
				SetConsoleTextAttribute(hConsole, 79);
				if (showType) cout << "[ERROR]";
				break;
			case 2:
				SetConsoleTextAttribute(hConsole, 111);
				if (showType) cout << "[WARNING]";
				break;
			case 3:
				SetConsoleTextAttribute(hConsole, 143);
				if (showType) cout << "[DEBUG]";
				break;
			}

			if (p_time) {
				time_t now = time(0);
				tm *ltm = localtime(&now);
				cout << "["
					<< setfill('0') << setw(2) << ltm->tm_hour << ":"
					<< setfill('0') << setw(2) << ltm->tm_min << ":"
					<< setfill('0') << setw(2) << ltm->tm_sec << "]";
			}


			cout << " " << msg;
			if (p_newline) cout << std::endl;
			SetConsoleTextAttribute(hConsole, 15);
		}
	}
}
