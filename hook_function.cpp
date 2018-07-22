#include "stdafx.h"
#include "hook_function.h"
#include "lib/drawing/draw.h"

//IMGUI Library
#include "lib/imgui/imgui.h"
#include "lib/imgui/imgui_impl_dx9.h"

//68 ªª®v head
//68 122
#define characterHEAD ((NumVertices == 68 && PrimitiveCount!=80) || (NumVertices == 122 && PrimitiveCount!=140) || NumVertices  == 114|| NumVertices == 282 || NumVertices == 74 || NumVertices == 194 || NumVertices == 34 || NumVertices == 26 || NumVertices == 158 ||NumVertices == 130 ||NumVertices == 254 ||NumVertices == 66 || NumVertices == 82 ||NumVertices == 50)
PDWORD Direct3D_VMTable = NULL;

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

int aimheight = 0;
D3DVIEWPORT9 g_ViewPort;

class CDraw CDraw;


struct ModelInfo_t
{
	D3DXVECTOR3 Position2D;
	D3DXVECTOR3 Position3D;
	float Distance;
	D3DPRIMITIVETYPE Type;
	INT BaseIndex; 
	UINT MinIndex; 
	UINT NumVertices; 
	UINT StartIndex; 
	UINT PrimitiveCount;
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

HRESULT WINAPI CreateDevice_Detour(LPDIRECT3D9 Direct3D_Object, UINT Adapter, D3DDEVTYPE DeviceType, HWND FocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* PresentationParameters, LPDIRECT3DDEVICE9* Returned_Device_Interface)
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

void AddModel(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE Type, INT BaseIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount)
{
	ModelInfo_t* pModel = new ModelInfo_t;

	pDevice->GetViewport(&g_ViewPort);

	D3DXMATRIX pProjection, pView, pWorld;
	D3DXVECTOR3 vOut(0, 0, 0), vIn(0, (float)aimheight, 1);

	pDevice->GetVertexShaderConstantF(0, pProjection, 4);
	pDevice->GetVertexShaderConstantF(230, pView, 4);

	D3DXMatrixIdentity(&pWorld);

	D3DXVECTOR3 VectorMiddle(0, 0, 0), ScreenMiddlee(0, 0, 0);
	D3DXVec3Unproject(&VectorMiddle, &ScreenMiddlee, &g_ViewPort, &pProjection, &pView, &pWorld);

	D3DXVec3Project(&vOut, &vIn, &g_ViewPort, &pProjection, &pView, &pWorld);

	float RealDistance = GetDistance(VectorMiddle.x, VectorMiddle.y, vIn.x, vIn.y) / 100;

	if (vOut.z < 1.0f)
	{
		pModel->Position2D.y = vOut.y;
		pModel->Position2D.x = vOut.x;
		pModel->Position2D.z = vOut.z;
		pModel->Distance = RealDistance;
		pModel->Type = Type;
		pModel->BaseIndex = BaseIndex;
		pModel->MinIndex = MinIndex;
		pModel->NumVertices = NumVertices;
		pModel->StartIndex = StartIndex;
		pModel->PrimitiveCount = PrimitiveCount;
	}

	ModelInfo.push_back(pModel);
}

int foundnum = 0;
float mouseOffset_X = 0, mouseOffset_Y = 0;
float ScreenCenterX = NULL, ScreenCenterY = NULL;
float mouseSmooth=2.5, minCrosshairDistance = 500;
ModelInfo_t* targetModel = new ModelInfo_t;
ModelInfo_t* focusModel = new ModelInfo_t;

HRESULT WINAPI EndScene_Detour(LPDIRECT3DDEVICE9 pDevice)
{
	if (g_font_default == NULL) {
		//Create fonts
		D3DXCreateFont(pDevice, 15, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, (LPCWSTR)"Arial", &g_font_default);
	}

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


	foundnum = 0;
	minCrosshairDistance = 500;

	
	if (ModelInfo.size() != NULL)
	{
		D3DDEVICE_CREATION_PARAMETERS cparams;
		RECT rect;
		pDevice->GetCreationParameters(&cparams);
		GetWindowRect(cparams.hFocusWindow, &rect);
		if (ScreenCenterX == NULL) ScreenCenterX = (rect.right - rect.left) / 2.0f;
		if (ScreenCenterY == NULL) ScreenCenterY = (rect.bottom - rect.top) / 2.0f;

		for (size_t i = 0; i < ModelInfo.size(); i++)
		{
			
			PrintText(g_font_default, (int)ModelInfo[i]->Position2D.x - 10, (int)ModelInfo[i]->Position2D.y , D3DCOLOR_XRGB(255, 0, 0),
				"%.1f m",
				ModelInfo[i]->Distance);
			
			if (minCrosshairDistance > GetDistance(ModelInfo[i]->Position2D.x, ModelInfo[i]->Position2D.y, ScreenCenterX, ScreenCenterY)
				&& GetDistance(ModelInfo[i]->Position2D.x, ModelInfo[i]->Position2D.y, ScreenCenterX, ScreenCenterY) < 300 ) {
				targetModel = ModelInfo[i];
				minCrosshairDistance = GetDistance(ModelInfo[i]->Position2D.x, ModelInfo[i]->Position2D.y, ScreenCenterX, ScreenCenterY);
				foundnum++;
			}

		}

		ModelInfo.clear();
	}

	if (GetAsyncKeyState(0x4) && foundnum > 0 ) {

		//PrintText(g_font_default, minX, minY, D3DCOLOR_XRGB(0, 255, 0), "Target");
		CDraw.Circle(targetModel->Position2D.x, targetModel->Position2D.y, 15, 0, full, true, 4, LAWNGREEN(255));
		
		mouseOffset_X = (targetModel->Position2D.x - ScreenCenterX) / mouseSmooth;
		mouseOffset_Y = (targetModel->Position2D.y - ScreenCenterY + 17) / mouseSmooth;

		if (mouseOffset_X >= 50)
			mouseOffset_X = (targetModel->Position2D.x - ScreenCenterX) / ((mouseSmooth * 0.5f) < 1 ? 1 : (mouseSmooth * 0.5f));

		if (mouseOffset_Y >= 50)
			mouseOffset_Y = (targetModel->Position2D.y - ScreenCenterY + 17) / ((mouseSmooth * 0.5f) < 1 ? 1 : (mouseSmooth * 0.5f));

		mouseOffset_X += 2;

		printf("ScreenCenterX:%f ScreenCenterY:%f\n", ScreenCenterX, ScreenCenterY);
		printf("minX:%f minY:%f minDistance:%f\n", targetModel->Position2D.x, targetModel->Position2D.y, minCrosshairDistance);
		printf("mouseOffset X:%f Y:%f (actual: X:%f Y:%f)\n", (targetModel->Position2D.x - ScreenCenterX), (targetModel->Position2D.y - ScreenCenterY), mouseOffset_X, mouseOffset_Y);
		printf("\n");
		
		mouse_event(MOUSEEVENTF_MOVE, mouseOffset_X, mouseOffset_Y , 0, 0);
		
	}
	

	return EndScene_Pointer(pDevice);
}

HRESULT WINAPI DrawIndexedPrimitive_Detour(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE Type, INT BaseIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount)
{
	LPDIRECT3DVERTEXBUFFER9 Stream_Data;
	UINT Offset, Stride;

	if (pDevice->GetStreamSource(0, &Stream_Data, &Offset, &Stride) == D3D_OK) Stream_Data->Release();

	if (func_wallhack && Stride == 32 && StartIndex == 0)
	{
		wallhack_ghostChams(pDevice, Type, BaseIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);
		if (characterHEAD) {
			AddModel(pDevice, Type, BaseIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);
		}
	}

	if (GetAsyncKeyState(VK_NUMPAD2) & 1)
	{
		mouseSmooth += 0.1f;
		cout << "mouseSmooth:" << mouseSmooth << endl;
	}

	if (GetAsyncKeyState(VK_NUMPAD8) & 1)
	{
		mouseSmooth -= 0.1f;
		cout << "mouseSmooth:" << mouseSmooth << endl;
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
	*(PDWORD)&Direct3D_VMTable[42] = (DWORD)EndScene_Detour;
	*(PDWORD)&Direct3D_VMTable[82] = (DWORD)DrawIndexedPrimitive_Detour;
	*(PDWORD)&Direct3D_VMTable[118] = (DWORD)CreateQuery_Detour;
}