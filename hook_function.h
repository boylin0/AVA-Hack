#pragma once
#include "stdafx.h"
#include "d3dMenu.h"
#include "logConsole.h"

extern bool menu;
extern bool func_wallhack;
extern class d3dMenu d3dmenu;
extern class logConsole console;

HRESULT WINAPI Direct3DCreate9_VMTable(VOID);
DWORD WINAPI VirtualMethodTableRepatchingLoopToCounterExtensionRepatching(LPVOID Param);

void hookD3Dfunction();