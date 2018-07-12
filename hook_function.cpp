#include "stdafx.h"
#include "hook_function.h"


PDWORD Direct3D_VMTable = NULL;
bool fCall = true;

typedef HRESULT(WINAPI* CreateDevice_Prototype)          (LPDIRECT3D9, UINT, D3DDEVTYPE, HWND, DWORD, D3DPRESENT_PARAMETERS*, LPDIRECT3DDEVICE9*);
typedef HRESULT(WINAPI* Reset_Prototype)                 (LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
typedef HRESULT(WINAPI* EndScene_Prototype)              (LPDIRECT3DDEVICE9);
typedef HRESULT(WINAPI* DrawIndexedPrimitive_Prototype)  (LPDIRECT3DDEVICE9, D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT);
typedef HRESULT(WINAPI* CreateQuery_Prototype)           (LPDIRECT3DDEVICE9, D3DQUERYTYPE, IDirect3DQuery9**);

CreateDevice_Prototype         CreateDevice_Pointer = NULL;
Reset_Prototype                Reset_Pointer = NULL;
EndScene_Prototype             EndScene_Pointer = NULL;
DrawIndexedPrimitive_Prototype DrawIndexedPrimitive_Pointer = NULL;
CreateQuery_Prototype          CreateQuery_Pointer = NULL;

LPD3DXFONT g_font_default;
LPDIRECT3DTEXTURE9 texture_Red, texture_Black;
IDirect3DPixelShader9 *Front, *Back;

HRESULT WINAPI Direct3DCreate9_VMTable(VOID)
{
	console.startConsole();
	console.logMessage("console started...", 0);

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

HRESULT WINAPI CreateDevice_Detour(LPDIRECT3D9 Direct3D_Object, UINT Adapter, D3DDEVTYPE DeviceType, HWND FocusWindow,
	DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* PresentationParameters,
	LPDIRECT3DDEVICE9* Returned_Device_Interface)
{
	HRESULT Returned_Result = CreateDevice_Pointer(Direct3D_Object, Adapter, DeviceType, FocusWindow, BehaviorFlags,
		PresentationParameters, Returned_Device_Interface);

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

	if (Returned_Result == D3D_OK)
	{
		Direct3D_VMTable = (PDWORD)*(PDWORD)*Returned_Device_Interface;

		*(PDWORD)&Reset_Pointer = (DWORD)Direct3D_VMTable[16];
		*(PDWORD)&EndScene_Pointer = (DWORD)Direct3D_VMTable[42];
		*(PDWORD)&DrawIndexedPrimitive_Pointer = (DWORD)Direct3D_VMTable[82];
		*(PDWORD)&CreateQuery_Pointer = (DWORD)Direct3D_VMTable[118];

		if (CreateThread(NULL, 0, VirtualMethodTableRepatchingLoopToCounterExtensionRepatching, NULL, 0, NULL) == NULL)
			return D3DERR_INVALIDCALL;
	}

	return Returned_Result;
}

HRESULT WINAPI Reset_Detour(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	g_font_default->OnLostDevice();
	g_font_default->OnResetDevice();

	return Reset_Pointer(pDevice, pPresentationParameters);
}

HRESULT GenerateTexture(IDirect3DDevice9 *pD3Ddev, IDirect3DTexture9 **ppD3Dtex, DWORD colour32)
{
	if (FAILED(pD3Ddev->CreateTexture(8, 8, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, ppD3Dtex, NULL)))
		return E_FAIL;

	WORD colour16 = ((WORD)((colour32 >> 28) & 0xF) << 12)
		| (WORD)(((colour32 >> 20) & 0xF) << 8)
		| (WORD)(((colour32 >> 12) & 0xF) << 4)
		| (WORD)(((colour32 >> 4) & 0xF) << 0);
	D3DLOCKED_RECT d3dlr;
	(*ppD3Dtex)->LockRect(0, &d3dlr, 0, 0);
	WORD *pDst16 = (WORD*)d3dlr.pBits;
	for (int xy = 0; xy < 8 * 8; xy++)
		*pDst16++ = colour16;
	(*ppD3Dtex)->UnlockRect(0);
	return S_OK;
}

HRESULT CreateMyShader(IDirect3DPixelShader9 **pShader, IDirect3DDevice9 *Device, float red, float green, float blue, float alpha)
{
	ID3DXBuffer *MyBuffer = NULL;
	char MyShader[256];
	sprintf(MyShader, "ps.1.1\ndef c0, %f, %f, %f, %f\nmov r0,c0", red / 255, green / 255, blue / 255, alpha / 255);
	D3DXAssembleShader(MyShader, sizeof(MyShader), NULL, NULL, 0, &MyBuffer, NULL);
	if (FAILED(Device->CreatePixelShader((const DWORD*)MyBuffer->GetBufferPointer(), pShader)))return E_FAIL;
	return S_OK;
}



HRESULT WINAPI EndScene_Detour(LPDIRECT3DDEVICE9 pDevice)
{
	if (texture_Red == NULL) GenerateTexture(pDevice, &texture_Red, D3DCOLOR_ARGB(255, 255, 0, 0));
	if (texture_Black == NULL) GenerateTexture(pDevice, &texture_Black, D3DCOLOR_ARGB(255, 0, 0, 0));
	if (g_font_default == NULL) D3DXCreateFont(pDevice, 15, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, (LPCWSTR)"Arial", &g_font_default); //Create fonts

	
	if (d3dmenu.g_font == NULL) {
		d3dmenu.g_font = g_font_default;
		d3dmenu.additem("AVA Hack @2018", D3DCOLOR_ARGB(255, 255, 000, 000));
		d3dmenu.additem("[MENU]", D3DCOLOR_ARGB(255, 255, 000, 000));
		d3dmenu.additem("[F4] WallHack", D3DCOLOR_ARGB(255, 255, 000, 000));
		d3dmenu.additem("[F5] ChangeName", D3DCOLOR_ARGB(255, 255, 000, 000));
		d3dmenu.additem("[F6] QQMacro", D3DCOLOR_ARGB(255, 255, 000, 000));
	}
	

	if (menu)
	{
		/*
		DrawString(1, 1, D3DCOLOR_ARGB(255, 000, 255, 000), "AVA HACK @2018");
		DrawString(1, 40, D3DCOLOR_ARGB(255, 000, 255, 000), "[MENU]");
		DWORD colorGreen = D3DCOLOR_ARGB(255, 000, 255, 000), colorRed = D3DCOLOR_ARGB(255, 255, 000, 000);
		if (func_wallhack) DrawString(1, 60, colorGreen, "WallHack [On]"); else DrawString(1, 60, colorRed ,"WallHack [Off]");
		if (func_changename) DrawString(1, 80, colorGreen, "ChangeName [On]"); else DrawString(1, 80, colorRed, "ChangeName [Off]");
		*/

		d3dmenu.drawMenu();
	}
	return EndScene_Pointer(pDevice);
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

void wallhack_default(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE Type, INT BaseIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount) {
	DWORD dwOldZEnable;
	//get previous state
	pDevice->GetRenderState(D3DRS_ZENABLE, &dwOldZEnable);
	//set state
	pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	DrawIndexedPrimitive_Pointer(pDevice, Type, BaseIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);

	//set back
	pDevice->SetRenderState(D3DRS_ZENABLE, dwOldZEnable);
}



HRESULT WINAPI DrawIndexedPrimitive_Detour(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE Type, INT BaseIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount)
{
	LPDIRECT3DVERTEXBUFFER9 Stream_Data;
	UINT Offset, Stride;

	if (pDevice->GetStreamSource(0, &Stream_Data, &Offset, &Stride) == D3D_OK) Stream_Data->Release();

	if (fCall)
	{
		CreateMyShader(&Front, pDevice, 255, 0, 0, 255);
		CreateMyShader(&Back, pDevice, 0, 255, 0, 255);
		fCall = false;
	}


	if (func_wallhack && Stride == 32 && StartIndex == 0)
	{
		wallhack_ghostChams(pDevice, Type, BaseIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);
	}

	return DrawIndexedPrimitive_Pointer(pDevice, Type, BaseIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);
}

HRESULT WINAPI CreateQuery_Detour(LPDIRECT3DDEVICE9 pDevice, D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery)
{

		if (Type == D3DQUERYTYPE_OCCLUSION)
		{
			Type = D3DQUERYTYPE_TIMESTAMP;
		}
	
	return CreateQuery_Pointer(pDevice, Type, ppQuery);
}

void hookD3Dfunction() {
	*(PDWORD)&Direct3D_VMTable[16] = (DWORD)Reset_Detour;
	*(PDWORD)&Direct3D_VMTable[42] = (DWORD)EndScene_Detour;
	*(PDWORD)&Direct3D_VMTable[82] = (DWORD)DrawIndexedPrimitive_Detour;
	*(PDWORD)&Direct3D_VMTable[118] = (DWORD)CreateQuery_Detour;
}