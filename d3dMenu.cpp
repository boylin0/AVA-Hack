#include "stdafx.h"
#include "d3dMenu.h"


void d3dMenu::drawMenu() {
	for (size_t i = 0; i < item_word.size(); i++)
	{
		DrawString(1, 1+(i*20), item_color[i], item_word[i]);
	}
}

void d3dMenu::additem(char* word, DWORD color) {
	item_word.push_back(word);
	item_color.push_back(color);
}

void d3dMenu::moditem(int id,char* word, DWORD color) {
	item_word[id] = word;
	item_color[id] = color;
}

/*
void PrintText(char pString[], int x, int y, D3DCOLOR col, ID3DXFont *font)
{
    RECT FontRect = { x, y, x+500, y+30 };
    font->DrawText( NULL, pString, -1, &FontRect, DT_LEFT | DT_WORDBREAK, col);
}
*/

void d3dMenu::DrawString(int x, int y, DWORD color, const char *fmt, ...)
{
	RECT FontPos = { x, y, x + 30, y + 20 };
	char buf[1024] = { '\0' };
	va_list va_alist;
	va_start(va_alist, fmt);
	vsprintf(buf, fmt, va_alist);
	va_end(va_alist);
	g_font->DrawTextA(NULL, buf, -1, &FontPos, DT_NOCLIP, color);
}
