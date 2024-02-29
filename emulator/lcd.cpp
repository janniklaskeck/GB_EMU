#include "lcd.h"

#include <memory>

using namespace GB;

bool LCD::IsControlAddress(u16 address)
{
	return address == 0xFF40;
}

bool LCD::IsStatusAddress(u16 address)
{
	return address == 0xFF41 || address == 0xFF44 || address == 0xFF45;
}

bool LCD::IsPosScrollAddress(u16 address)
{
	return address == 0xFF42 || address == 0xFF43 || address == 0xFF4A || address == 0xFF4B;
}

bool LCD::IsBCPSAddress(u16 address)
{
	return address >= 0xFF47 && address <= 0xFF49;
}

u8 LCD::ReadByte(u16 address)
{
	if (address == 0xFF44)
	{
		return 0x90;
	}

	return 0;
}
