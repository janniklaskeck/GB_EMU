#include <timer.h>
#include "emu.h"
#include "cpu.h"

using namespace GB;

Timer::Timer()
{
	div = 0xABCC;
}

void Timer::Tick(const u32 cycleAmount)
{
	divCounter += cycleAmount;
	if (divCounter >= 255)
	{
		divCounter = 0;
		div++;
	}

	if (tac & (1 << 2))
	{
		counter -= cycleAmount;
		if (counter <= 0)
		{
			UpdateClockFrequency();

			if (tima == 0xFF)
			{
				tima = tma;
				EMU::GetEMU()->GetCPU()->RequestInterrupt(IntType::IT_Timer);
			}
			else
			{
				tima++;
			}
		}
	}
}

void Timer::WriteByte(u16 address, u8 value)
{
	switch (address)
	{
	case 0xFF04: div = 0; return;
	case 0xFF05: tima = value; return;
	case 0xFF06: tma = value; return;
	case 0xFF07:
	{
		const u8 currentFrequency = GetClockFrequency();
		tac = value;
		const u8 newFrequency = GetClockFrequency();

		if (currentFrequency != newFrequency)
		{
			UpdateClockFrequency();
		}

		return;
	}
	default:
	{
		exit(-7);
		return;
	}
	}
}

u8 Timer::ReadByte(u16 address)
{
	switch (address)
	{
	case 0xFF04: return div >> 8;
	case 0xFF05: return tima;
	case 0xFF06: return tma;
	case 0xFF07: return tac;
	default:
	{
		exit(-7);
		return 0;
	}
	}
}

bool Timer::IsTimer_Addr(u16 address)
{
	return address >= 0xFF04 && address <= 0xFF07;
}

u8 Timer::GetClockFrequency() const
{
	return tac & 0x3;
}

void Timer::UpdateClockFrequency()
{
	u8 freq = GetClockFrequency();
	switch (freq)
	{
	case 0: counter = 1024; break;	// freq 4096
	case 1: counter = 16; break;	// freq 262144
	case 2: counter = 64; break;	// freq 65536
	case 3: counter = 256; break;	// freq 16382
	}
}
