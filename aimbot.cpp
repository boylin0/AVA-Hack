
#include "aimbot.h"
#include "lib/drawing/draw.h"
#include <vector>
#include "newMenu.h"
#include "utils.h"

//zombie
/*
|| pModel->NumVertices == 785 && pModel->PrimitiveCount == 997 \
|| pModel->NumVertices == 4387 && pModel->PrimitiveCount == 5992 \
|| pModel->NumVertices == 2829 && pModel->PrimitiveCount == 3931 \
|| pModel->NumVertices == 3571 && pModel->PrimitiveCount == 5557 \
|| pModel->NumVertices == 2076 && pModel->PrimitiveCount == 2981 \
|| pModel->NumVertices == 2683 && pModel->PrimitiveCount == 3993 \
*/

#define fixHead (\
   pModel->NumVertices == 473 && pModel->PrimitiveCount == 134 \
|| pModel->NumVertices == 882 && pModel->PrimitiveCount == 304 \
)


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
		float minCrosshairDistance = 500;

		D3DVIEWPORT9 g_ViewPort;
		

		ModelInfo_t* targetModel = new ModelInfo_t;
		ModelInfo_t* focusModel = new ModelInfo_t;
		vector<ModelInfo_t*>ModelInfo;

		float getDistance(float Xx, float Yy, float xX, float yY) {
			return sqrt((yY - Yy) * (yY - Yy) + (xX - Xx) * (xX - Xx));
		}

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
			D3DXVECTOR3 vOut(0, 0, 0), vIn(0, 0, (float)menu::item::slider_aimheight);

			pDevice->GetVertexShaderConstantF(0, pProjection, 4);
			pDevice->GetVertexShaderConstantF(230, pView, 4);

			D3DXMatrixIdentity(&pWorld);

			D3DXVECTOR3 VectorMiddle(0, 0, 0), ScreenMiddlee(0, 0, 0);
			D3DXVec3Unproject(&VectorMiddle, &ScreenMiddlee, &g_ViewPort, &pProjection, &pView, &pWorld);

			D3DXVec3Project(&vOut, &vIn, &g_ViewPort, &pProjection, &pView, &pWorld);

			float RealDistance = getDistance(VectorMiddle.x, VectorMiddle.y, vIn.x, vIn.y) / 100;

			if (vOut.z < 1.0f) {
				pModel->Position2D.y = vOut.y;
				pModel->Position2D.x = vOut.x;
				pModel->Position2D.z = vOut.z;
				pModel->Distance = RealDistance; //m
				pModel->Type = Type;
				pModel->BaseIndex = BaseIndex;
				pModel->MinIndex = MinIndex;
				pModel->NumVertices = NumVertices;
				pModel->StartIndex = StartIndex;
				pModel->PrimitiveCount = PrimitiveCount;

				if (fixHead) {
					pModel->Position2D.y -= 50;
				}
					

				ModelInfo.push_back(pModel);
			}
		}

		void SearchTarget(LPDIRECT3DDEVICE9 pDevice) {

			isFoundTarget = false;
			minCrosshairDistance = 600;


			if (ModelInfo.size() != NULL)
			{
				D3DDEVICE_CREATION_PARAMETERS cparams;
				RECT rect;
				pDevice->GetCreationParameters(&cparams);
				GetWindowRect(cparams.hFocusWindow, &rect);
				
				if (ScreenCenterX == NULL || ScreenCenterX != (rect.right - rect.left) / 2.0f) ScreenCenterX = (rect.right - rect.left) / 2.0f;
				if (ScreenCenterY == NULL || ScreenCenterY != (rect.bottom - rect.top) / 2.0f) ScreenCenterY = (rect.bottom - rect.top) / 2.0f;

				for (size_t i = 0; i < ModelInfo.size(); i++)
				{
					PrintText(g_font_default, (int)ModelInfo[i]->Position2D.x - 10, (int)ModelInfo[i]->Position2D.y, D3DCOLOR_XRGB(255, 0, 0),
						"%.1f m",
						ModelInfo[i]->Distance);
#ifdef _DEBUG
					if ( (GetAsyncKeyState(0x45)) && menu::item::checkbox_debugMode ) {
						PrintText(g_font_default, (int)ModelInfo[i]->Position2D.x - 10, (int)ModelInfo[i]->Position2D.y + 20, D3DCOLOR_XRGB(255, 0, 0),
							"NumVertices: %d\nPrimitiveCount: %d",
							ModelInfo[i]->NumVertices,
							ModelInfo[i]->PrimitiveCount);
					}
#endif
					// check is found target
					if (isFocusTargetSet) {
						if (ModelInfo[i]->BaseIndex == focusModel->BaseIndex
							&& ModelInfo[i]->MinIndex == focusModel->MinIndex
							&& ModelInfo[i]->StartIndex == focusModel->StartIndex
							&& ModelInfo[i]->Type == focusModel->Type
							&& ModelInfo[i]->NumVertices == focusModel->NumVertices
							&& ModelInfo[i]->PrimitiveCount == focusModel->PrimitiveCount
							&& minCrosshairDistance > getDistance(ModelInfo[i]->Position2D.x, ModelInfo[i]->Position2D.y, ScreenCenterX, ScreenCenterY)
							&& getDistance(ModelInfo[i]->Position2D.x, ModelInfo[i]->Position2D.y, ScreenCenterX, ScreenCenterY) < 300
							&& (ModelInfo[i]->Distance < focusModel->Distance + 2.0f && ModelInfo[i]->Distance > focusModel->Distance - 2.0f)) {
							focusModel->Distance = ModelInfo[i]->Distance;
							targetModel = ModelInfo[i];
							minCrosshairDistance = getDistance(ModelInfo[i]->Position2D.x, ModelInfo[i]->Position2D.y, ScreenCenterX, ScreenCenterY);
							isFoundTarget = true;
						}

					}
					// find closet target relative to crosshair
					else if (minCrosshairDistance > getDistance(ModelInfo[i]->Position2D.x, ModelInfo[i]->Position2D.y, ScreenCenterX, ScreenCenterY)
						&& getDistance(ModelInfo[i]->Position2D.x, ModelInfo[i]->Position2D.y, ScreenCenterX, ScreenCenterY) < 300) {
						targetModel = ModelInfo[i];
						minCrosshairDistance = getDistance(ModelInfo[i]->Position2D.x, ModelInfo[i]->Position2D.y, ScreenCenterX, ScreenCenterY);
						isFoundTarget = true;
					}

				}

			}
		}

		void doAim() {
			if (GetAsyncKeyState(menu::item::aimbotKEY) && utils::isFocusOnAVA() && !menu::isMENU) {
				if (isFoundTarget) {
#ifdef _DEBUG
					printf("%d in file %s\n", __LINE__, __FILE__);
					printf("targetModel->Position2D.x:%d targetModel->Position2D.y:%d", targetModel->Position2D.x, targetModel->Position2D.y);
#endif // DEBUG
					//PrintText(g_font_default, minX, minY, D3DCOLOR_XRGB(0, 255, 0), "Target");
					CDraw.Circle((int)targetModel->Position2D.x, (int)targetModel->Position2D.y, 15, 0, full, true, 4, LAWNGREEN(255));
					//printf("%d in file %s\n", __LINE__, __FILE__);
					if(menu::item::checkbox_debugMode)
#ifdef _DEBUG
					PrintText(g_font_default, (int)targetModel->Position2D.x - 10, (int)targetModel->Position2D.y + 20, LAWNGREEN(255),
						"NumVertices: %d\nPrimitiveCount: %d",
						targetModel->NumVertices,
						targetModel->PrimitiveCount);
#endif // DEBUG
					mouseOffset_X = (targetModel->Position2D.x - ScreenCenterX +  3 ) / menu::item::slider_aimspeed;
					mouseOffset_Y = (targetModel->Position2D.y - ScreenCenterY + 14 ) / menu::item::slider_aimspeed;
					//printf("targetModel->Position2D.x:%f %d %d %f\n", targetModel->Position2D.x, ScreenCenterX, (targetModel->Position2D.x - ScreenCenterX + 3), menu::item::slider_aimspeed);
					//printf("mouseOffset_X_calc:%f\n", (targetModel->Position2D.x - ScreenCenterX + 3) / menu::item::slider_aimspeed);
					mouseOffset_X = 100; // hack fix for X not working...
					//printf("mouseOffset_X:%d\n", mouseOffset_X);
					if (mouseOffset_X >= 50)
						mouseOffset_X = int((targetModel->Position2D.x - ScreenCenterX +  3 ) / ((menu::item::slider_aimspeed * 0.55f) < 1 ? 1 : (menu::item::slider_aimspeed * 0.55f)) + 0.5);
					//if (mouseOffset_X >= 3 && mouseOffset_X <= 50)
					//mouseOffset_X = int((targetModel->Position2D.x - ScreenCenterX + 3) / ((mouseSmooth * 0.7f) < 1 ? 1 : (mouseSmooth * 0.7f)) + 0.5);

					if (mouseOffset_Y >= 50)
						mouseOffset_Y = int((targetModel->Position2D.y - ScreenCenterY + 14 ) / ((menu::item::slider_aimspeed * 0.55f) < 1 ? 1 : (menu::item::slider_aimspeed * 0.55f)) + 0.5);
					//if (mouseOffset_X >= 3 && mouseOffset_X <= 50)
					//mouseOffset_Y = int((targetModel->Position2D.y - ScreenCenterY + 14) / ((mouseSmooth * 0.7f) < 1 ? 1 : (mouseSmooth * 0.7f)) + 0.5);

#ifdef _DEBUG
					printf("ScreenCenterX:%d ScreenCenterY:%d\n", ScreenCenterX, ScreenCenterY);
					printf("minX:%f minY:%f minDistance:%f\n", targetModel->Position2D.x, targetModel->Position2D.y, minCrosshairDistance);
					printf("mouseOffset X:%d Y:%d (actual: X:%d Y:%d)\n", int(targetModel->Position2D.x - ScreenCenterX), int(targetModel->Position2D.y - ScreenCenterY), mouseOffset_X, mouseOffset_Y);
					printf("\n");
#endif // DEBUG

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

		int isSecondAdjustment = 3;
		float pre_ModelInfo_X = 0;
		float pre_ModelInfo_Y = 0;

		void Release() {
			ModelInfo.clear();
		}


	}//namespace aimbot
}//namespace hack
