#pragma once
#include "stdafx.h"

extern LPD3DXFONT g_font_default;
extern class CDraw CDraw;


namespace function {
	namespace aimbot {
		void AddModel(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE Type, INT BaseIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount);
		void SearchTarget(LPDIRECT3DDEVICE9 pDevice);
		void doAim();
	}
}