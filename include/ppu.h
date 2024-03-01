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

	constexpr int LINES_PER_FRAME = 154;
	constexpr int TICKS_PER_LINE = 456;
	constexpr int YRES = 144;
	constexpr int XRES = 160;

	constexpr u32 target_frame_time = 1000 / 60;

	class PPU
	{
	public:

		PPU() = default;

		void Tick();

		u32 GetCurrentFrame() const
		{
			return current_frame;
		}

		u8 ReadOAM_Byte(u16 address) const;
		void WriteOAM_Byte(u16 address, u8 value);

		static bool IsOAM_Addr(u16 address);

	private:

		void Tick_OAM();

		void Tick_XFER();

		void Tick_VLBANK();

		void TICK_HBLANK();

	private:

		u32 current_frame = 0;
		u32 line_ticks = 0;

		long prev_frame_time = 0;
		long start_timer = 0;
		long frame_count = 0;

		std::array<u8, TOTAL_OAM_SIZE> oam = {};

		std::array<u32, XRES * YRES> videoBuffer = {};
	};
}