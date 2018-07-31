#pragma once
#include "stdafx.h"

#include "logConsole.h"
#include "Memory.h"

 
extern class d3dMenu d3dmenu;
extern class Memory Memory;
extern class logConsole console;

extern HANDLE hProcess ;
extern DWORD baseAddress ;

bool isFocusOnAVA();
void DoQQMacro();
void DoChangeName();
