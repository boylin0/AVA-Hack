#include "stdafx.h"
#include "hook_function.h"
#include "lib/drawing/draw.h"
#include "aimbot.h"
#include "newMenu.h"

//IMGUI Library
#include "lib/imgui/imgui.h"
#include "lib/imgui/imgui_impl_dx9.h"

//68 ªª®v head
//68 122
#define characterHEAD ((NumVertices == 68 && PrimitiveCount!=80) || (NumVertices == 122 && PrimitiveCount!=140) || NumVertices  == 114|| NumVertices == 282 || NumVertices == 74 || NumVertices == 194 || NumVertices == 34 || NumVertices == 26 || NumVertices == 158 ||NumVertices == 130 ||NumVertices == 254 ||NumVertices == 66 || NumVertices == 82 ||NumVertices == 50)

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


bool testbool;



HRESULT WINAPI Present_Detour(IDirect3DDevice9 *pDevice, CONST RECT *pSourceRect, CONST RECT *pDestRect, HWND hDestWindowOverride, CONST RGNDATA *pDirtyRegion)
{
	menu::initMenu(pDevice);
	return Present_Pointer(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}


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
	CDraw.GetDevice(pDevice);
	g_font_default->OnLostDevice();
	g_font_default->OnResetDevice();

	//free resources
	ImGui_ImplDX9_InvalidateDeviceObjects();
	ImGui_ImplDX9_CreateDeviceObjects();
	return Reset_Pointer(pDevice, pPresentationParameters);
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


HRESULT WINAPI BeginScene_Detour(LPDIRECT3DDEVICE9 *pDevice)
{
	return BeginScene_Pointer(pDevice);
}

HRESULT WINAPI EndScene_Detour(LPDIRECT3DDEVICE9 pDevice)
{
	menu::MenuRender();

	if (g_font_default == NULL) {
		//Create fonts
		D3DXCreateFont(pDevice, 15, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, (LPCWSTR)"Arial", &g_font_default);
	}

	function::aimbot::SearchTarget(pDevice);
	function::aimbot::doAim();

	if (GetAsyncKeyState(VK_HOME) & 0x1) {
		menu::isMENU = !menu::isMENU;
	}

	return EndScene_Pointer(pDevice);
}

HRESULT WINAPI DrawIndexedPrimitive_Detour(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE Type, INT BaseIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount)
{
	LPDIRECT3DVERTEXBUFFER9 Stream_Data;
	UINT Offset, Stride;

	if (pDevice->GetStreamSource(0, &Stream_Data, &Offset, &Stride) == D3D_OK) Stream_Data->Release();

	if (menu::item::checkbox_wallhack && Stride == 32 && StartIndex == 0)
	{
		wallhack_ghostChams(pDevice, Type, BaseIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);
		if (characterHEAD) {
			function::aimbot::AddModel(pDevice, Type, BaseIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);
		}
	}

	if (GetAsyncKeyState(VK_NUMPAD2) & 1)
	{
		function::aimbot::mouseSmooth += 0.1f;
		std::cout << "mouseSmooth:" << function::aimbot::mouseSmooth << endl;
	}

	if (GetAsyncKeyState(VK_NUMPAD8) & 1)
	{
		function::aimbot::mouseSmooth -= 0.1f;
		std::cout << "mouseSmooth:" << function::aimbot::mouseSmooth << endl;
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