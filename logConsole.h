#pragma once

namespace function {
	namespace console {

		enum loginfo_type {
			info = 0,
			error = 1,
			warning = 2,
			debug = 3
		};

		void start();
		void message(char *msg, DWORD type, bool showType = true, bool p_time = true, bool p_newline = true);
	}
}



