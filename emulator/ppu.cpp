#include <ppu.h>

using namespace GB;

u8 PPU::ReadOAM_Byte(u16 address) const
{
	const u16 translatedAddress = address - OAM_START;

	return oam[translatedAddress];
}

void PPU::WriteOAM_Byte(u16 address, u8 value)
{
	const u16 translatedAddress = address - OAM_START;
	oam[translatedAddress] = value;
}

bool PPU::IsOAM_Addr(u16 address)
{
	return address >= OAM_START && address <= OAM_END;
}