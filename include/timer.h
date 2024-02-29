#pragma once

#include <common.h>

namespace GB
{
#define CLOCKSPEED 4194304

	class Timer
	{

	public:

		Timer();

	public:

		void Tick(const u32 cycleAmount);

		void WriteByte(u16 address, u8 value);

		u8 ReadByte(u16 address);

		static bool IsTimer_Addr(u16 address);

	private:

		u8 GetClockFrequency() const;

		void UpdateClockFrequency();

	private:

		u16 div = 0;
		u8 tima = 0;
		u8 tma = 0;
		u8 tac = 0;

		i32 counter = 1024;
		u16 divCounter = 0;
	};
}

void timer_init();
void timer_tick();
