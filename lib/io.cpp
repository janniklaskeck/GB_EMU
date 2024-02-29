#include <io.h>
#include "timer.h"
#include "emu.h"
#include "cpu.h"
#include "ppu.h"
#include "bus.h"

using namespace GB;

u8 IO::ReadByte(u16 address)
{
	if (address == 0xFF01)
	{
		return serialData[0];
	}

	if (address == 0xFF02)
	{
		return serialData[1];
	}

	if (LCD::IsBCPSAddress(address))
	{
		return lcd.ReadByte(address);
	}

	if (LCD::IsControlAddress(address))
	{
		return lcd.ReadByte(address);
	}

	if (LCD::IsPosScrollAddress(address))
	{
		return lcd.ReadByte(address);
	}

	if (LCD::IsStatusAddress(address))
	{
		return lcd.ReadByte(address);
	}

	if (Timer::IsTimer_Addr(address))
	{
		return EMU::GetTimer()->ReadByte(address);
	}

	if (address == 0xFF0F)
	{
		return EMU::GetCPU()->GetInterruptFlags();
	}

	if (address == 0xFFFF)
	{
		return EMU::GetCPU()->GetInterruptEnabledFlags();
	}

	if (PPU::IsOAM_Addr(address))
	{
		if (EMU::GetBUS()->DMA_TransferActive())
		{
			return 0xFF;
		}

		return EMU::GetPPU()->ReadOAM_Byte(address);
	}

	if (address == 0xFF44)
	{
		static u8 a;
		return a++;
	}

	return 0;
}

void IO::WriteByte(u16 address, u8 value)
{
	if (address == 0xFF01)
	{
		serialData[0] = value;
		return;
	}

	if (address == 0xFF02)
	{
		serialData[1] = value;
		return;
	}

	if (Timer::IsTimer_Addr(address))
	{
		EMU::GetTimer()->WriteByte(address, value);
		return;
	}

	if (address == 0xFF0F)
	{
		EMU::GetCPU()->SetInterruptFlags(value);
		return;
	}

	if (address == 0xFF46)
	{
		EMU::GetBUS()->DMA_Start(value);
		return;
	}

	if (PPU::IsOAM_Addr(address))
	{
		if (EMU::GetBUS()->DMA_TransferActive())
		{
			return;
		}

		EMU::GetPPU()->WriteOAM_Byte(address, value);
		return;
	}
}

bool IO::IsIO_Addr(u16 address)
{
	return (address >= 0xFF00 && address < 0xFF80) || address == 0xFFFF;
}

