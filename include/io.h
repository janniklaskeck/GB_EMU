#pragma once

#include <common.h>
#include "lcd.h"

#include <array>

namespace GB
{
	class IO
	{

	public:

		IO() = default;

	public:

		u8 ReadByte(u16 address);

		void WriteByte(u16 address, u8 value);

		static bool IsIO_Addr(u16 address);

	private:

		LCD lcd;

		std::array<u8, 2> serialData{};
	};
}
