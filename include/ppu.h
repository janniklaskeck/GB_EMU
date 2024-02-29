#pragma once

#include <common.h>
#include <array>

namespace GB
{
	// OAM
	constexpr u16 TOTAL_OAM_SIZE = 0xA0;
	constexpr u16 OAM_START = 0xFE00;
	constexpr u16 OAM_END = OAM_START + TOTAL_OAM_SIZE - 1;
	// ~OAM

	class PPU
	{
	public:

		u8 ReadOAM_Byte(u16 address) const;
		void WriteOAM_Byte(u16 address, u8 value);

		static bool IsOAM_Addr(u16 address);

	private:
		std::array<u8, TOTAL_OAM_SIZE> oam;
	};
}