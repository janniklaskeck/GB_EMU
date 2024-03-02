#include "lcd.h"

#include <memory>
#include "emu.h"
#include "bus.h"
#include "cpu.h"

using namespace GB;

constexpr std::array<u32, 4> DefaultColors = { 0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000 };

LCD::LCD()
{
	controlFlags = 0x91;
	DMG_BG_Pallette = 0xFC;
	DMG_OBJ_Pallettes[0] = 0xFF;
	DMG_OBJ_Pallettes[1] = 0xFF;

	DMG_BG_Colors = DefaultColors;
	DMG_SP1_Colors = DefaultColors;
	DMG_SP2_Colors = DefaultColors;

	Set_PPU_Mode(LCD_Mode::OAM);
}

u8 LCD::ReadByte(u16 address)
{
	const u8 offset = address - 0xFF40;
	u8* thisBytePtr = (u8*)this;

	return *(thisBytePtr + offset);
}

void LCD::WriteByte(u16 address, u8 value)
{
	const u8 offset = address - 0xFF40;
	u8* thisBytePtr = (u8*)this;
	thisBytePtr[offset] = value;

	if (offset == 6)
	{
		EMU::GetBUS()->DMA_Start(value);
		return;
	}

	if (address >= 0xFF47 && address <= 0xFF49)
	{
		const u8 palletteIndex = address - 0xFF47;
		UpdatePallette(value, palletteIndex);
		return;
	}
}

bool LCD::IsLCDAddress(u16 address)
{
	return address >= 0xFF40 && address <= 0xFF4B;
}

void LCD::UpdatePallette(u8 palletteData, u8 pallette)
{
	u32* colorPtr = nullptr;

	switch (pallette)
	{
	case 1:
	{
		colorPtr = DMG_SP1_Colors.data();
		palletteData &= 0b11111100;
		break;
	}
	case 2:
	{
		colorPtr = DMG_SP2_Colors.data();
		palletteData &= 0b11111100;
		break;
	}
	default:
	{
		colorPtr = DMG_BG_Colors.data();
		break;
	}
	}

	colorPtr[0] = DefaultColors[palletteData & 0b11];
	colorPtr[1] = DefaultColors[(palletteData >> 2) & 0b11];
	colorPtr[2] = DefaultColors[(palletteData >> 4) & 0b11];
	colorPtr[3] = DefaultColors[(palletteData >> 6) & 0b11];
}

void LCD::IncrementLY()
{
	ly++;

	if (ly == lyCompare)
	{
		lyCompare = 1;

		if (Get_Int_Src_Enabled(LCDS_Int_Src::LYC))
		{
			EMU::GetCPU()->RequestInterrupt(IntType::IT_LCD_Stat);
		}
	}
	else
	{
		lyCompare = 0;
	}
}
