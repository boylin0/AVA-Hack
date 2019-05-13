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
			extern bool  checkbox_debugMode;
			extern bool  checkbox_unlimitAmmo;
			extern bool  checkbox_noSpread;
			extern bool  checkbox_UAV;
			extern bool  checkbox_SuperWeapon;
			extern bool  checkbox_antiReport;
			extern bool  checkbox_fastReload;
			extern int   slider_Rank;
			extern float slider_aimspeed;
			extern float slider_aimheight;
			extern char  input_ChangeName[30];
		}
		extern bool isMENU;
		void MenuRender();
		void initMenu(IDirect3DDevice9 *pDevice);
	}
}
