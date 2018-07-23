#pragma once
#include "stdafx.h"
#include "d3dMenu.h"

using namespace std;

class d3dMenu {
public:
	void drawMenu();
	void additem(char* word, DWORD color);
	void moditem(int id, char* word, DWORD color);
	LPD3DXFONT g_font = NULL;
private:
	void DrawString(int x, int y, DWORD color, const char *fmt, ...);
	vector<DWORD> item_color;
	vector<char*> item_word;
};