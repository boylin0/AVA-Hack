#pragma once
#include "stdafx.h"

namespace function {
	namespace menu {
		namespace item {
			extern bool  checkbox_wallhack;
			extern bool  checkbox_aimbot;
			extern bool  checkbox_QQMacro;
			extern bool  checkbox_ChangeName;
			extern bool  checkbox_ChangeRank;
			extern int   slider_Rank;
			extern float slider_aimheight;
			//extern char input_ChangeName[];
		}
		extern bool isMENU;
		void MenuRender();
		void initMenu(IDirect3DDevice9 *pDevice);
	}
}
