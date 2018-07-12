#pragma once
#include "stdafx.h"
#include "d3dMenu.h"


extern bool menu;
extern bool func_wallhack;
extern class d3dMenu d3dmenu;

HRESULT WINAPI Direct3DCreate9_VMTable(VOID);
HRESULT WINAPI CreateDevice_Detour(LPDIRECT3D9, UINT, D3DDEVTYPE, HWND, DWORD, D3DPRESENT_PARAMETERS*, LPDIRECT3DDEVICE9*);
HRESULT WINAPI Reset_Detour(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
HRESULT WINAPI EndScene_Detour(LPDIRECT3DDEVICE9);
HRESULT WINAPI DrawIndexedPrimitive_Detour(LPDIRECT3DDEVICE9, D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT);
HRESULT WINAPI CreateQuery_Detour(LPDIRECT3DDEVICE9, D3DQUERYTYPE, IDirect3DQuery9**);
extern DWORD WINAPI VirtualMethodTableRepatchingLoopToCounterExtensionRepatching(LPVOID Param);

void hookD3Dfunction();