#pragma once

#include "common.h"
#include <array>

namespace GB
{
	class LCD
	{

	public:
		LCD();

		u8 ReadByte(u16 address);

		void WriteByte(u16 address, u8 value);

		static bool IsLCDAddress(u16 address);

	public:

		// Control flags

		// LCD& PPU enable : 0 = Off; 1 = On
		// Window tile map area : 0 = 9800–9BFF; 1 = 9C00–9FFF
		// Window enable : 0 = Off; 1 = On
		// BG & Window tile data area : 0 = 8800–97FF; 1 = 8000–8FFF
		// BG tile map area : 0 = 9800–9BFF; 1 = 9C00–9FFF
		// OBJ size : 0 = 8×8; 1 = 8×16
		// OBJ enable : 0 = Off; 1 = On
		// BG & Window enable / priority[Different meaning in CGB Mode]: 0 = Off; 1 = On

		u8 Get_BG_Window_Enable_Prio() const
		{
			return BIT(controlFlags, 0);
		}

		u8 Get_OBJ_Enable() const
		{
			return BIT(controlFlags, 1);
		}

		u8 Get_OBJ_Size() const
		{
			return BIT(controlFlags, 2) ? 16 : 8;
		}

		u8 Get_BG_TileMap() const
		{
			return BIT(controlFlags, 3) ? 0x9C00 : 0x9800;
		}

		u8 Get_BG_Window_Tiles() const
		{
			return BIT(controlFlags, 4) ? 0x8000 : 0x8800;
		}

		u8 Get_Window_Enable() const
		{
			return BIT(controlFlags, 5);
		}

		u8 Get_Window_TileMap() const
		{
			return BIT(controlFlags, 6) ? 0x9C00 : 0x9800;
		}

		u8 Get_LCD_PPU_Enable() const
		{
			return BIT(controlFlags, 7);
		}

		u8 GetControlFlags() const
		{
			return controlFlags;
		}

		void WriteControlFlags(u8 newValue)
		{
			controlFlags = newValue;
		}

		// Status flags

		// LYC int select(Read / Write) : If set, selects the LYC == LY condition for the STAT interrupt.
		// Mode 2 int select(Read / Write) : If set, selects the Mode 2 condition for the STAT interrupt.
		// Mode 1 int select(Read / Write) : If set, selects the Mode 1 condition for the STAT interrupt.
		// Mode 0 int select(Read / Write) : If set, selects the Mode 0 condition for the STAT interrupt.
		// LYC == LY(Read - only) : Set when LY contains the same value as LYC; it is constantly updated.
		// PPU mode(Read - only) : Indicates the PPU’s current status.

		LCD_Mode Get_PPU_Mode() const
		{
			return (LCD_Mode)(statusFlags & 0b11);
		}

		void Set_PPU_Mode(const LCD_Mode mode)
		{
			u8 modeByte = (u8)mode;
			statusFlags &= ~0b11;
			statusFlags |= modeByte;
		}

		u8 Get_LYC_EQ_LY() const
		{
			return BIT(statusFlags, 2);
		}

		u8 Get_Int_Src_Enabled(const LCDS_Int_Src source) const
		{
			return statusFlags & (u8)source;
		}

		u8 GetStatusFlags() const
		{
			return statusFlags;
		}

		void WriteStatusFlags(u8 newValue)
		{
			statusFlags = newValue;
		}

		u8 GetLY() const
		{
			return ly;
		}

		void SetLY(u8 newValue)
		{
			ly = newValue;
		}

		void IncrementLY();

	private:

		void UpdatePallette(u8 palletteData, u8 pallette);

	private:

		u8 controlFlags = 0;
		u8 statusFlags = 0;

		u8 scrollX = 0;
		u8 scrollY = 0;

		u8 ly = 0;
		u8 lyCompare = 0;

		u8 dma = 0;

		// DMG only
		u8 DMG_BG_Pallette;
		std::array<u8, 2> DMG_OBJ_Pallettes;
		// ~DMG only

		u8 windowX = 0;
		u8 windowY = 0;

		using DMG_Pallette_Colors = std::array<u32, 4>;

		DMG_Pallette_Colors DMG_BG_Colors;
		DMG_Pallette_Colors DMG_SP1_Colors;
		DMG_Pallette_Colors DMG_SP2_Colors;

		//CGB
		struct CGB_BCPS
		{
			u8 address;
			u8 autoIncrement;
		};

		CGB_BCPS CGB_bcps{};

		struct CGB_Color
		{
			u16 RedIntensity;
			u16 GreenIntensity;
			u16 BlueIntensity;
		};

		struct CGB_Pallette
		{
			CGB_Color c0{};
			CGB_Color c1{};
			CGB_Color c2{};
			CGB_Color c3{};
		};

		std::array<CGB_Pallette, 8> CGB_BG_Pallettes{};
		std::array<CGB_Pallette, 8> CGB_OBJ_Pallettes{};
	};
}