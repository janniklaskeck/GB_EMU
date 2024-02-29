
#include "ram.h"

using namespace GB;

void RAM::WriteWRAM_Byte(u16 address, u8 value)
{
	u16 translatedAddress = address - RAM_ADDR::WRAM_START;
	if (translatedAddress < RAM_ADDR::WRAM_BANK_SIZE)
	{
		WRAM_Bank_0[translatedAddress] = value;
	}
	else
	{
		translatedAddress -= RAM_ADDR::WRAM_BANK_SIZE;
		WRAM_Banks_1_7[currentWRAMBank][translatedAddress] = value;
	}
}

u8 RAM::ReadWRAM_Byte(u16 address)
{
	u16 translatedAddress = address - RAM_ADDR::WRAM_START;
	if (translatedAddress < RAM_ADDR::WRAM_BANK_SIZE)
	{
		return WRAM_Bank_0[translatedAddress];
	}

	translatedAddress -= RAM_ADDR::WRAM_BANK_SIZE;
	return WRAM_Banks_1_7[currentWRAMBank][translatedAddress];
}

void RAM::WriteVRAM_Byte(u16 address, u8 value)
{
	const u16 translatedAddress = address - RAM_ADDR::VRAM_START;
	VRAM_Banks[currentVRAMBank][translatedAddress] = value;
}

u8 RAM::ReadVRAM_Byte(u16 address)
{
	const u16 translatedAddress = address - RAM_ADDR::VRAM_START;
	return VRAM_Banks[currentVRAMBank][translatedAddress];
}

void RAM::WriteEXT_RAM_Byte(u16 address, u8 value)
{

}

u8 RAM::ReadEXT_RAM_Byte(u16 address)
{
	return 0;
}

void RAM::WriteHRAM_Byte(u16 address, u8 value)
{
	const u16 translatedAddress = address - RAM_ADDR::HRAM_START;
	HRAM_Mem[translatedAddress] = value;
}

u8 RAM::ReadHRAM_Byte(u16 address)
{
	const u16 translatedAddress = address - RAM_ADDR::HRAM_START;
	return HRAM_Mem[translatedAddress];
}

bool RAM::IsWRAM_Addr(u16 address)
{
	return address >= RAM_ADDR::WRAM_START && address <= RAM_ADDR::WRAM_END;
}

bool RAM::IsVRAM_Addr(u16 address)
{
	return address >= RAM_ADDR::VRAM_START && address <= RAM_ADDR::VRAM_END;
}

bool RAM::IsHRAM_Addr(u16 address)
{
	return address >= RAM_ADDR::HRAM_START && address <= RAM_ADDR::HRAM_END;
}
