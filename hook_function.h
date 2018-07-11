#pragma once
#include "stdafx.h"
#include "d3dMenu.h"

typedef HRESULT(WINAPI* CreateDevice_Prototype)          (LPDIRECT3D9, UINT, D3DDEVTYPE, HWND, DWORD, D3DPRESENT_PARAMETERS*, LPDIRECT3DDEVICE9*);
typedef HRESULT(WINAPI* Reset_Prototype)                 (LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
typedef HRESULT(WINAPI* EndScene_Prototype)              (LPDIRECT3DDEVICE9);
typedef HRESULT(WINAPI* DrawIndexedPrimitive_Prototype)  (LPDIRECT3DDEVICE9, D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT);
typedef HRESULT(WINAPI* CreateQuery_Prototype)           (LPDIRECT3DDEVICE9, D3DQUERYTYPE, IDirect3DQuery9**);

extern CreateDevice_Prototype         CreateDevice_Pointer;
extern Reset_Prototype                Reset_Pointer;
extern EndScene_Prototype             EndScene_Pointer;
extern DrawIndexedPrimitive_Prototype DrawIndexedPrimitive_Pointer;
extern CreateQuery_Prototype          CreateQuery_Pointer;

extern PDWORD Direct3D_VMTable;

extern LPD3DXFONT g_font_default ;
extern LPDIRECT3DTEXTURE9 texture_Red , texture_Black ;
extern IDirect3DPixelShader9 *Front, *Back;
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