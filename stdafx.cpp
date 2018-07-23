// stdafx.cpp : source file that includes just the standard includes
// xinput.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

namespace function {
	namespace common {
		float getDistance(float Xx, float Yy, float xX, float yY)
		{
			return sqrt((yY - Yy) * (yY - Yy) + (xX - Xx) * (xX - Xx));
		}
	}
}
