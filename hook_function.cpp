#include "stdafx.h"
#include "hook_function.h"
//68 ªª®v head
//68 122
#define chahead ((NumVertices == 68 && PrimitiveCount!=80) || (NumVertices == 122 && PrimitiveCount!=140) || NumVertices  == 114|| NumVertices == 282 || NumVertices == 74 || NumVertices == 194 || NumVertices == 34 || NumVertices == 26 || NumVertices == 158 ||NumVertices == 130 ||NumVertices == 254 ||NumVertices == 66 || NumVertices == 82 ||NumVertices == 50)
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


int aimheight = 0;
D3DVIEWPORT9 g_ViewPort;

struct ModelInfo_t
{
	D3DXVECTOR3 Position2D;
	D3DXVECTOR3 Position3D;
	float Distance;
};

vector<ModelInfo_t*>ModelInfo;

void PrintText(LPD3DXFONT Font, long x, long y, D3DCOLOR fontColor, char *text, ...)
{
	RECT rct;
	rct.left = x - 1;
	rct.right = x + 1;
	rct.top = y - 1;
	rct.bottom = y + 1;

	if (!text) { return; }

	char buf[256] = { 0 };
	RECT FontRect = { x, y, x, y };
	va_list va_alist;
	va_start(va_alist, text);
	vsprintf(buf, text, va_alist);
	va_end(va_alist);
	g_font_default->DrawTextA(NULL, buf, -1, &rct, DT_NOCLIP, fontColor);
}

float GetDistance(float Xx, float Yy, float xX, float yY)
{
	return sqrt((yY - Yy) * (yY - Yy) + (xX - Xx) * (xX - Xx));
}

void DrawPoint(LPDIRECT3DDEVICE9 pDevice, int baseX, int baseY, int baseW, int baseH, D3DCOLOR Cor)
{
	D3DRECT BarRect = { baseX, baseY, baseX + baseW, baseY + baseH };
	pDevice->Clear(1, &BarRect, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, Cor, 0, 0);
}

void AddModel(LPDIRECT3DDEVICE9 pDevice)
{

	ModelInfo_t* pModel = new ModelInfo_t;
	
	pDevice->GetViewport(&g_ViewPort);
	D3DXMATRIX pProjection, pView, pWorld;
	D3DXVECTOR3 vOut(0, 0, 0), vIn(0, (float)aimheight, 1);

	pDevice->GetVertexShaderConstantF(0, pProjection, 4);
	pDevice->GetVertexShaderConstantF(231, pView, 4);

	D3DXMatrixIdentity(&pWorld);

	D3DXVECTOR3 VectorMiddle(0, 0,0), ScreenMiddlee(0, 0, 0);
	D3DXVec3Unproject(&VectorMiddle, &ScreenMiddlee, &g_ViewPort, &pProjection, &pView, &pWorld);

	D3DXVec3Project(&vOut, &vIn, &g_ViewPort, &pProjection, &pView, &pWorld);

	float RealDistance = GetDistance(VectorMiddle.x, VectorMiddle.y, vIn.x, vIn.y) / 100;

	if (vOut.z < 1.0f)
	{
		
		pModel->Position2D.y = vOut.y;
		pModel->Position2D.x = vOut.x;
		pModel->Position2D.z = vOut.z;
		pModel->Distance = RealDistance;
	}

	ModelInfo.push_back(pModel);
}


HRESULT WINAPI Direct3DCreate9_VMTable(VOID)
{
	console.startConsole();
	console.logMessage("Console Started...\n", 0);

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

int minX, minY, minDistance, foundnum=0;
int mouseOffset_X, mouseOffset_Y;
bool isfirst = true;
int ScreenCenterX = NULL, ScreenCenterY = NULL;
float mou=2.5;


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

	if (GetAsyncKeyState(0x4) && foundnum>0) {
		printf("ScreenCenterX:%d ScreenCenterY:%d\n", ScreenCenterX, ScreenCenterY);
		printf("minX:%d minY:%d minDistance:%d\n", minX, minY, minDistance);
		printf("mouseOffset X:%d Y:%d (actual: X:%d Y:%d)\n", (minX - ScreenCenterX), (minY - ScreenCenterY) ,  (minX - ScreenCenterX )/ mou , (minY - ScreenCenterY) / mou);
		printf("\n");
		PrintText(g_font_default, minX, minY, D3DCOLOR_XRGB(0, 255, 0), "Target");

		mouseOffset_X = (minX - ScreenCenterX) / mou;
		mouseOffset_Y = (minY - ScreenCenterY + 13) / mou;
		if (mouseOffset_X >= 50) mouseOffset_X = (minX - ScreenCenterX) / ((mou * 0.5) < 1 ? 1 : (mou * 0.5));
		if (mouseOffset_Y >= 50) mouseOffset_Y = (minY - ScreenCenterY) / ((mou * 0.5) < 1 ? 1 : (mou * 0.5));
		mouse_event(MOUSEEVENTF_MOVE, mouseOffset_X, mouseOffset_Y, 0, 0);

	}

	foundnum = 0;
	minDistance = 500;
	return EndScene_Pointer(pDevice);
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
		DWORD dwOldZEnable = D3DZB_TRUE;
		pDevice->GetRenderState(D3DRS_ZENABLE, &dwOldZEnable);
		pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		DrawIndexedPrimitive_Pointer(pDevice, Type, BaseIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);
		pDevice->SetRenderState(D3DRS_ZENABLE, dwOldZEnable);

		if (GetAsyncKeyState(VK_F8) || chahead) {
			AddModel(pDevice);
			if (ModelInfo.size() != NULL)
			{
				D3DDEVICE_CREATION_PARAMETERS cparams;
				RECT rect;
				pDevice->GetCreationParameters(&cparams);
				GetWindowRect(cparams.hFocusWindow, &rect);
				if (ScreenCenterX == NULL) ScreenCenterX = (rect.right - rect.left ) / 2.0f;
				if (ScreenCenterY == NULL) ScreenCenterY = (rect.bottom - rect.top) / 2.0f;

				for (size_t i = 0; i < ModelInfo.size(); i++)
				{

					DrawPoint(pDevice, (int)ModelInfo[i]->Position2D.x, (int)ModelInfo[i]->Position2D.y, 4, 4, D3DCOLOR_XRGB(255, 0, 0));
					PrintText(g_font_default, (int)ModelInfo[i]->Position2D.x, (int)ModelInfo[i]->Position2D.y - 15, D3DCOLOR_XRGB(255, 0, 0),
						"NumVertices: %d\nPrimitiveCount: %d",
						NumVertices,
						PrimitiveCount);

					if (GetAsyncKeyState(VK_F7)) cout << NumVertices << endl;
					/*
					"d: %0.1f m\nX:%f Y:%f\ndistance:%f",
					ModelInfo[i]->Distance,
					ModelInfo[i]->Position2D.x,
					ModelInfo[i]->Position2D.y,
					GetDistance(ModelInfo[i]->Position2D.x, ModelInfo[i]->Position2D.y, ScreenCenterX, ScreenCenterY));
					*/

					if ( minDistance > GetDistance(ModelInfo[i]->Position2D.x, ModelInfo[i]->Position2D.y, ScreenCenterX, ScreenCenterY) && GetDistance(ModelInfo[i]->Position2D.x, ModelInfo[i]->Position2D.y, ScreenCenterX, ScreenCenterY) < 300) {
						minX = ModelInfo[i]->Position2D.x;
						minY = ModelInfo[i]->Position2D.y;
						minDistance = GetDistance(ModelInfo[i]->Position2D.x, ModelInfo[i]->Position2D.y, ScreenCenterX, ScreenCenterY);
						foundnum++;
					}

				}


				ModelInfo.clear();
			}
		}


		//wallhack_ghostChams(pDevice, Type, BaseIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);
	}


	if (GetAsyncKeyState(VK_NUMPAD2) & 1)
	{
		mou += 0.1;
		cout << "mousespeed:" << mou << endl;
	}

	if (GetAsyncKeyState(VK_NUMPAD8) & 1)
	{
		mou -= 0.1;
		cout << "mousespeed:" << mou << endl;
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