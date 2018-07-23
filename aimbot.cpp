
#include "aimbot.h"
#include "lib/drawing/draw.h"
#include <vector>

using namespace function::common;

namespace function {
	namespace aimbot {
	

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

		bool isFoundTarget = false, isFocusTargetSet = false;
		int mouseOffset_X = 0, mouseOffset_Y = 0, ScreenCenterX = NULL, ScreenCenterY = NULL;
		float mouseSmooth = 4, minCrosshairDistance = 500;

		D3DVIEWPORT9 g_ViewPort;
		int aimheight = 0;

		ModelInfo_t* targetModel = new ModelInfo_t;
		ModelInfo_t* focusModel = new ModelInfo_t;
		vector<ModelInfo_t*>ModelInfo;

		void PrintText(LPD3DXFONT gFont, long x, long y, D3DCOLOR fontColor, char *text, ...)
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
			gFont->DrawTextA(NULL, buf, -1, &rct, DT_NOCLIP, fontColor);
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

			float RealDistance = getDistance(VectorMiddle.x, VectorMiddle.y, vIn.x, vIn.y) / 100;

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

		void SearchTarget(LPDIRECT3DDEVICE9 pDevice) {

			isFoundTarget = false;
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

					PrintText(g_font_default, (int)ModelInfo[i]->Position2D.x - 10, (int)ModelInfo[i]->Position2D.y, D3DCOLOR_XRGB(255, 0, 0),
						"%.1f m",
						ModelInfo[i]->Distance);

					if (isFocusTargetSet) {
						if (ModelInfo[i]->BaseIndex == focusModel->BaseIndex
							&& ModelInfo[i]->MinIndex == focusModel->MinIndex
							&& ModelInfo[i]->StartIndex == focusModel->StartIndex
							&& ModelInfo[i]->Type == focusModel->Type
							&& ModelInfo[i]->NumVertices == focusModel->NumVertices
							&& ModelInfo[i]->PrimitiveCount == focusModel->PrimitiveCount
							&& minCrosshairDistance > getDistance(ModelInfo[i]->Position2D.x, ModelInfo[i]->Position2D.y, ScreenCenterX, ScreenCenterY)
							&& getDistance(ModelInfo[i]->Position2D.x, ModelInfo[i]->Position2D.y, ScreenCenterX, ScreenCenterY) < 300) {
							targetModel = ModelInfo[i];
							minCrosshairDistance = getDistance(ModelInfo[i]->Position2D.x, ModelInfo[i]->Position2D.y, ScreenCenterX, ScreenCenterY);
							isFoundTarget = true;
						}

					}
					else if (minCrosshairDistance > getDistance(ModelInfo[i]->Position2D.x, ModelInfo[i]->Position2D.y, ScreenCenterX, ScreenCenterY)
						&& getDistance(ModelInfo[i]->Position2D.x, ModelInfo[i]->Position2D.y, ScreenCenterX, ScreenCenterY) < 300) {
						targetModel = ModelInfo[i];
						minCrosshairDistance = getDistance(ModelInfo[i]->Position2D.x, ModelInfo[i]->Position2D.y, ScreenCenterX, ScreenCenterY);
						//isFocusTargetSet = false; // new FocusTarget
						isFoundTarget = true;
					}

				}

				ModelInfo.clear();
			}
		}

		void doAim() {
			if (GetAsyncKeyState(0x4)) {
				if (isFoundTarget) {
					//PrintText(g_font_default, minX, minY, D3DCOLOR_XRGB(0, 255, 0), "Target");
					CDraw.Circle(targetModel->Position2D.x, targetModel->Position2D.y, 15, 0, full, true, 4, LAWNGREEN(255));

					mouseOffset_X = (targetModel->Position2D.x - ScreenCenterX +  3 ) / mouseSmooth;
					mouseOffset_Y = (targetModel->Position2D.y - ScreenCenterY + 14 ) / mouseSmooth;

					if (mouseOffset_X >= 50)
						mouseOffset_X = int((targetModel->Position2D.x - ScreenCenterX +  3 ) / ((mouseSmooth * 0.5f) < 1 ? 1 : (mouseSmooth * 0.5f)) + 0.5);

					if (mouseOffset_Y >= 50)
						mouseOffset_Y = int((targetModel->Position2D.y - ScreenCenterY + 14 ) / ((mouseSmooth * 0.5f) < 1 ? 1 : (mouseSmooth * 0.5f)) + 0.5);



					printf("ScreenCenterX:%d ScreenCenterY:%d\n", ScreenCenterX, ScreenCenterY);
					printf("minX:%f minY:%f minDistance:%f\n", targetModel->Position2D.x, targetModel->Position2D.y, minCrosshairDistance);
					printf("mouseOffset X:%d Y:%d (actual: X:%d Y:%d)\n", int(targetModel->Position2D.x - ScreenCenterX), int(targetModel->Position2D.y - ScreenCenterY), mouseOffset_X, mouseOffset_Y);
					printf("\n");

					mouse_event(MOUSEEVENTF_MOVE, mouseOffset_X, mouseOffset_Y, 0, 0);
				}
				if (!isFocusTargetSet) {
					focusModel = targetModel;
					isFocusTargetSet = true;
				}
			}
			else {
				isFocusTargetSet = false;
			}
		}

	}//namespace aimbot
}//namespace hack
