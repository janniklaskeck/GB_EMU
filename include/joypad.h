#pragma once

#include "common.h"

namespace GB
{
	class Joypad
	{
		Joypad() = default;

		static Joypad* GetJoypad();

	public:

		static void WriteState(u8 newValue);

		static u8 ReadState();

		static bool IsJoypad_Addr(u16 address);

	private:

		u8 state = 0;
	};
}