#include "stdafx.h"
#include "hook_function.h"
#include "lib/drawing/draw.h"
#include "aimbot.h"
#include "newMenu.h"
#include "utils.h"

//IMGUI Library
#include "lib/imgui/imgui.h"
#include "lib/imgui/imgui_impl_dx9.h"

//Num | Pri
//282 520 | 245 385

//head
//60 38
//120 110
//60 58

//helmet
//|| (NumVertices == 3564 && PrimitiveCount == 16)
//|| (NumVertices == 1920 && PrimitiveCount == 574)
//|| (NumVertices == 5390 && PrimitiveCount == 132)
//|| (NumVertices == 2599 && PrimitiveCount == 2365)
//|| (NumVertices == 1290 && PrimitiveCount == 574) \
//|| (NumVertices == 1290 && PrimitiveCount == 558) \
//|| (NumVertices == 3748 && PrimitiveCount == 132) \
//|| (NumVertices == 1570 && PrimitiveCount == 624) \
//|| (NumVertices == 3248 && PrimitiveCount == 3796) \
//|| (NumVertices == 3564 && PrimitiveCount == 16) \
//|| (NumVertices == 1920 && PrimitiveCount == 574) \
//|| (NumVertices == 1771 && PrimitiveCount == 1664) \

//zombie
/*
|| NumVertices == 473 && PrimitiveCount == 134 \
|| NumVertices == 785 && PrimitiveCount == 997 \
|| NumVertices == 4387 && PrimitiveCount == 5992 \
|| NumVertices == 2829 && PrimitiveCount == 3931 \
|| NumVertices == 3571 && PrimitiveCount == 5557 \
|| NumVertices == 2076 && PrimitiveCount == 2981 \
|| NumVertices == 2683 && PrimitiveCount == 3993 \
*/

//head no added
//624 1570
//3854 3811
#define characterHEAD ( \
   NumVertices  == 114 \
|| NumVertices == 282 \
|| NumVertices == 74 \
|| NumVertices == 194 \
|| NumVertices == 34 \
|| NumVertices == 26 \
|| NumVertices == 158 \
|| NumVertices == 130 \
|| NumVertices == 254 \
|| NumVertices == 66 \
|| NumVertices == 82 \
|| NumVertices == 50 \
|| (NumVertices == 574 && PrimitiveCount == 539) \
|| (NumVertices == 122 && PrimitiveCount == 168) \
|| (NumVertices == 110 && PrimitiveCount == 120) \
|| (NumVertices == 58 && PrimitiveCount == 60) \
|| (NumVertices == 120 && PrimitiveCount == 110)  \
|| (NumVertices == 38 && PrimitiveCount == 60)  \
|| PrimitiveCount == 385 \
|| PrimitiveCount == 531 \
|| (NumVertices == 68 && PrimitiveCount!=80) \
|| (NumVertices == 122 && PrimitiveCount!=140) \
|| NumVertices == 882 && PrimitiveCount == 304 \
|| NumVertices == 473 && PrimitiveCount == 134 \
)

//|| (NumVertices == 158 && PrimitiveCount == 168)
#define unWanted (\
   (NumVertices == 8 && PrimitiveCount == 2) \
|| (NumVertices == 12 && PrimitiveCount == 2) \
|| (NumVertices == 1737 && PrimitiveCount == 844) \
|| (NumVertices == 3002 && PrimitiveCount == 1898) \
|| (NumVertices == 245 && PrimitiveCount == 385) \
)

using namespace function;

PDWORD Direct3D_VMTable = NULL;

typedef HRESULT(WINAPI* CreateDevice_Prototype)          (LPDIRECT3D9, UINT, D3DDEVTYPE, HWND, DWORD, D3DPRESENT_PARAMETERS*, LPDIRECT3DDEVICE9*);
typedef HRESULT(WINAPI* Reset_Prototype)                 (LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
typedef HRESULT(WINAPI* Present_Prototype)               (IDirect3DDevice9 *, CONST RECT *, CONST RECT *, HWND, CONST RGNDATA *);
typedef HRESULT(WINAPI * BeginScene_Prototype)           (LPDIRECT3DDEVICE9 *);
typedef HRESULT(WINAPI* EndScene_Prototype)              (LPDIRECT3DDEVICE9);
typedef HRESULT(WINAPI* DrawIndexedPrimitive_Prototype)  (LPDIRECT3DDEVICE9, D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT);
typedef HRESULT(WINAPI* CreateQuery_Prototype)           (LPDIRECT3DDEVICE9, D3DQUERYTYPE, IDirect3DQuery9**);

CreateDevice_Prototype         CreateDevice_Pointer = NULL;
Reset_Prototype                Reset_Pointer = NULL;
Present_Prototype              Present_Pointer = NULL;
BeginScene_Prototype           BeginScene_Pointer = NULL;
EndScene_Prototype             EndScene_Pointer = NULL;
DrawIndexedPrimitive_Prototype DrawIndexedPrimitive_Pointer = NULL;
CreateQuery_Prototype          CreateQuery_Pointer = NULL;

LPD3DXFONT g_font_default;
class CDraw CDraw;

HRESULT WINAPI CreateDevice_Detour(LPDIRECT3D9 Direct3D_Object, UINT Adapter, D3DDEVTYPE DeviceType, HWND FocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* PresentationParameters, LPDIRECT3DDEVICE9* Returned_Device_Interface)
{
	HRESULT Returned_Result = CreateDevice_Pointer(Direct3D_Object, Adapter, DeviceType, FocusWindow, BehaviorFlags, PresentationParameters, Returned_Device_Interface);
	DWORD dwProtect;
	if (VirtualProtect(&Direct3D_VMTable[16], sizeof(DWORD), PAGE_READWRITE, &dwProtect) != 0)
	{
		*(PDWORD)&Direct3D_VMTable[16] = *(PDWORD)&CreateDevice_Pointer;
		CreateDevice_Pointer = NULL;

		if (VirtualProtect(&Direct3D_VMTable[16], sizeof(DWORD), dwProtect, &dwProtect) == 0)
			return D3DERR_INVALIDCALL;
	}
	else
		return D3DERR_INVALIDCALL;

	if (Returned_Result == D3D_OK) {
		Direct3D_VMTable = (PDWORD)*(PDWORD)*Returned_Device_Interface;

		*(PDWORD)&Reset_Pointer = (DWORD)Direct3D_VMTable[16];
		*(PDWORD)&Present_Pointer = (DWORD)Direct3D_VMTable[17];
		*(PDWORD)&BeginScene_Pointer = (DWORD)Direct3D_VMTable[41];
		*(PDWORD)&EndScene_Pointer = (DWORD)Direct3D_VMTable[42];
		*(PDWORD)&DrawIndexedPrimitive_Pointer = (DWORD)Direct3D_VMTable[82];
		*(PDWORD)&CreateQuery_Pointer = (DWORD)Direct3D_VMTable[118];
		
		if (CreateThread(NULL, 0, VirtualMethodTableRepatchingLoopToCounterExtensionRepatching, NULL, 0, NULL) == NULL)
			return D3DERR_INVALIDCALL;
	}

	return Returned_Result;
}

HRESULT WINAPI Direct3DCreate9_VMTable(VOID)
{
	console::start();
	console::message("Console Started...\n", 0);

	LPDIRECT3D9 Direct3D_Object = Direct3DCreate9(D3D_SDK_VERSION);//get D3D DeviceObject Pointer

	if (Direct3D_Object == NULL)
		return D3DERR_INVALIDCALL;

	Direct3D_VMTable = (PDWORD)*(PDWORD)Direct3D_Object;
	Direct3D_Object->Release();

	DWORD dwProtect;

	if (VirtualProtect(&Direct3D_VMTable[16], sizeof(DWORD), PAGE_READWRITE, &dwProtect) != 0)
	{
		*(PDWORD)&CreateDevice_Pointer = Direct3D_VMTable[16];
		*(PDWORD)&Direct3D_VMTable[16] = (DWORD)CreateDevice_Detour;

		if (VirtualProtect(&Direct3D_VMTable[16], sizeof(DWORD), dwProtect, &dwProtect) == 0)
			return D3DERR_INVALIDCALL;
	}
	else
		return D3DERR_INVALIDCALL;

	return D3D_OK;
}



HRESULT WINAPI Reset_Detour(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	HRESULT r = NULL;
	//CDraw.GetDevice(pDevice);
	if(g_font_default != NULL) g_font_default->OnLostDevice();
	




	function::menu::isMENU = false;

	//free imgui resources
	ImGui_ImplDX9_InvalidateDeviceObjects();
	r = Reset_Pointer(pDevice, pPresentationParameters);
	ImGui_ImplDX9_CreateDeviceObjects();
	if (r == D3D_OK)
	{
		if (g_font_default != NULL)
			g_font_default->OnResetDevice();
	}

	return r;
}


void wallhack_ghostChams(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE Type, INT BaseIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount) {
	//Device_Interface->SetPixelShader(Back);

	DWORD dwOldZEnable, dwALPHABLENDENABLE, dwDESTBLEND, dwSRCBLEND;

	//get previous state
	pDevice->GetRenderState(D3DRS_ZENABLE, &dwOldZEnable);
	pDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwALPHABLENDENABLE);
	pDevice->GetRenderState(D3DRS_DESTBLEND, &dwDESTBLEND);
	pDevice->GetRenderState(D3DRS_SRCBLEND, &dwSRCBLEND);

	//set state
	pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVDESTCOLOR);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_INVSRCCOLOR);

	//pDevice->SetPixelShader(Front);
	DrawIndexedPrimitive_Pointer(pDevice, Type, BaseIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);

	//set back
	pDevice->SetRenderState(D3DRS_ZENABLE, dwOldZEnable);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, dwALPHABLENDENABLE);
	pDevice->SetRenderState(D3DRS_DESTBLEND, dwDESTBLEND);
	pDevice->SetRenderState(D3DRS_SRCBLEND, dwSRCBLEND);
	//Device_Interface->SetPixelShader(Back);
}

HRESULT WINAPI Present_Detour(IDirect3DDevice9 *pDevice, CONST RECT *pSourceRect, CONST RECT *pDestRect, HWND hDestWindowOverride, CONST RGNDATA *pDirtyRegion)
{
	menu::initMenu(pDevice);
	aimbot::Release();
	return Present_Pointer(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

HRESULT WINAPI BeginScene_Detour(LPDIRECT3DDEVICE9 *pDevice)
{
	return BeginScene_Pointer(pDevice);
}

HRESULT WINAPI EndScene_Detour(LPDIRECT3DDEVICE9 pDevice)
{
	menu::MenuRender();

	//create new font for drawing text
	if (g_font_default == NULL) {
		//Create fonts
		CDraw.GetDevice(pDevice);
		D3DXCreateFont(pDevice, 15, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, (LPCWSTR)"Arial", &g_font_default);
	}

	//do aimbot
	if (menu::item::checkbox_aimbot) {
		//find the best target then move crosshair to the target
		function::aimbot::SearchTarget(pDevice);
		function::aimbot::doAim();
	}

	//toggle menu
	if ( (GetAsyncKeyState(VK_HOME) & 0x1) && utils::isFocusOnAVA() ) {
		menu::isMENU = !menu::isMENU;
	}else if( menu::isMENU && utils::isFocusOnAVA() && (GetAsyncKeyState(VK_ESCAPE) & 0x1) ){
		menu::isMENU = false;
	}

	if (menu::item::checkbox_QQMacro) {
		D3DDEVICE_CREATION_PARAMETERS cparams;
		RECT rect;
		pDevice->GetCreationParameters(&cparams);
		GetWindowRect(cparams.hFocusWindow, &rect);
		CDraw.CircleFilled( ((rect.right - rect.left) / 2.0f) - 3.0f , ((rect.bottom - rect.top) / 2.0f) - 14.0f , 3, 0, full , 4, PURPLE(200) );
	}

	return EndScene_Pointer(pDevice);
}

HRESULT WINAPI DrawIndexedPrimitive_Detour(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE Type, INT BaseIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount)
{
	LPDIRECT3DVERTEXBUFFER9 Stream_Data;
	UINT Offset, Stride;

	if (pDevice->GetStreamSource(0, &Stream_Data, &Offset, &Stride) == D3D_OK) Stream_Data->Release();

	if ( (menu::item::checkbox_wallhack && (Stride == 32 || Stride == 88) && StartIndex == 0 && !unWanted) )
	{
		wallhack_ghostChams(pDevice, Type, BaseIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);

		//if model is head add to target list
		if (   (GetAsyncKeyState(0x45) && menu::item::checkbox_debugMode) 
			|| (characterHEAD && menu::item::checkbox_aimbot) ) {
			function::aimbot::AddModel(pDevice, Type, BaseIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);
		}
	}


	return DrawIndexedPrimitive_Pointer(pDevice, Type, BaseIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);
}

HRESULT WINAPI CreateQuery_Detour(LPDIRECT3DDEVICE9 pDevice, D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery)
{
	if (Type == D3DQUERYTYPE_OCCLUSION) {
		Type = D3DQUERYTYPE_TIMESTAMP;
	}
	return CreateQuery_Pointer(pDevice, Type, ppQuery);
}

void hookD3Dfunction() {
	*(PDWORD)&Direct3D_VMTable[16] = (DWORD)Reset_Detour;
	*(PDWORD)&Direct3D_VMTable[17] = (DWORD)Present_Detour;
	*(PDWORD)&Direct3D_VMTable[41] = (DWORD)BeginScene_Detour;
	*(PDWORD)&Direct3D_VMTable[42] = (DWORD)EndScene_Detour;
	*(PDWORD)&Direct3D_VMTable[82] = (DWORD)DrawIndexedPrimitive_Detour;
	*(PDWORD)&Direct3D_VMTable[118] = (DWORD)CreateQuery_Detour;
	
}