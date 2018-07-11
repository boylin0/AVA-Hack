#pragma once
#include "stdafx.h"

#include "d3dmenu.h"
#include "logConsole.h"
#include "Memory.h"

 
extern class d3dMenu d3dmenu;
extern class Memory Memory;
extern class logConsole console;

extern HANDLE hProcess;
extern DWORD baseAddress;

extern bool func_changename;
extern bool func_wallhack;
extern bool func_QQMacro;

extern BYTE orignal_name[72];

void DoQQMacro();
void ToggleQQMacro();
void ToggleChangeName();
void ToggleWallHack();