#pragma once

#include "common.h"
#include <array>

namespace GB
{
	class LCD
	{

	public:
		LCD() = default;


		u8 ReadByte(u16 address);

		static bool IsControlAddress(u16 address);

		static bool IsStatusAddress(u16 address);

		static bool IsPosScrollAddress(u16 address);

		static bool IsBCPSAddress(u16 address);

	private:

		// Control flags
		struct Control
		{
			u8 BG_Window_Enabled_Priority : 1;
			u8 objectEnabled : 1;
			u8 ObjectSize : 1;
			u8 BG_TileMapArea : 1;
			u8 BG_Window_TileDataArea : 1;
			u8 windowEnabled : 1;
			u8 Window_TileMapArea : 1;
			u8 enableLCD_PPU : 1;
		};

		Control controlFlags{};

		// Status flags
		struct Status
		{
			u8 PPU_Mode : 2;
			u8 LYC_Equals_LY : 1;
			u8 Mode0_int_select : 1;
			u8 Mode1_int_select : 1;
			u8 Mode2_int_select : 1;
			u8 LYC_int_select : 1;
			u8 dummy : 1;
		};

		Status statusFlags{};

		// PositionAndScrolling
		struct PosAndScroll
		{
			u8 sc_x;
			u8 sc_y;

			u8 w_x;
			u8 w_y;
		};

		PosAndScroll posAndScroll{};

		// Pallettes
		struct BCPS
		{
			u8 address : 6;
			u8 dummy : 1;
			u8 autoIncrement : 1;
		};

		BCPS bcps{};

		struct Color
		{
			u16 RedIntensity : 5;
			u16 GreenIntensity : 5;
			u16 BlueIntensity : 5;
			u16 dummy : 1;
		};

		struct Pallette
		{
			Color c0{};
			Color c1{};
			Color c2{};
			Color c3{};
		};

		std::array<Pallette, 8> BG_Pallets{};
		std::array<Pallette, 8> OBJ_Pallets{};
	};
}