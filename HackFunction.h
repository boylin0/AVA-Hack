#pragma once
#include "stdafx.h"

#include "d3dmenu.h"
#include "logConsole.h"
#include "Memory.h"

 
extern class d3dMenu d3dmenu;
extern class Memory Memory;
extern class logConsole console;

extern HANDLE hProcess ;
extern DWORD baseAddress ;
extern bool menu;

extern bool func_changename,
			func_wallhack,
			func_QQMacro;



bool isFocusOnAVA();
void DoQQMacro();
void ToggleQQMacro();
void ToggleChangeName();
void ToggleWallHack();