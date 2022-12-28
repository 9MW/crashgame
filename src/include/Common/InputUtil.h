#pragma once

#if PLATFORM_WIN322
#include"Win32/kwin.h"
#endif
#include"io/iodef.h"
#include"math0.h"
namespace pj2 {

	struct mousestate
	{
		using f2 = math::f2;
		union
		{
			f2 CursorPos;
			float PosX;
			float PosY;
		};

		enum BUTTON_FLAGS : int
		{
			BUTTON_FLAG_NONE = 0x00,
			BUTTON_FLAG_LEFT = 0x01,
			BUTTON_FLAG_MIDDLE = 0x02,
			BUTTON_FLAG_RIGHT = 0x04,
			BUTTON_FLAG_WHEEL = 0x08
		};
		int key;  int state;
		mousestate() { PosX = PosY = 0; key = state = -1; }
		mousestate(const mousestate& mst) { memcpy(this, &mst, sizeof(mst)); }
		operator f2& () { return CursorPos; }

	};
}