#pragma once

#include "common.h"
#include <array>

namespace GB
{
	namespace RAM_ADDR
	{
		// VRAM
		constexpr u16 VRAM_BANK_SIZE = 0x2000;
		constexpr u16 TOTAL_VRAM_SIZE = VRAM_BANK_SIZE * 2;

		constexpr u16 VRAM_BANK = 0x8000;

		constexpr u16 VRAM_START = VRAM_BANK;
		constexpr u16 VRAM_END = VRAM_BANK + VRAM_BANK_SIZE - 1;
		// ~VRAM

		// WRAM
		constexpr u16 WRAM_BANK_SIZE = 0x1000;
		constexpr u16 TOTAL_WRAM_SIZE = WRAM_BANK_SIZE * 8;

		constexpr u16 WRAM_BANK_0 = 0xC000;
		constexpr u16 WRAM_BANK_1_7 = 0xD000;

		constexpr u16 WRAM_START = WRAM_BANK_0;
		constexpr u16 WRAM_END = WRAM_BANK_0 + (WRAM_BANK_SIZE * 2) - 1;

		constexpr u16 WRAM_BANK_MIRROR_START = 0xE000;
		constexpr u16 WRAM_BANK_MIRROR_END = 0xFDFF;
		// ~WRAM

		// HRAM
		constexpr u16 TOTAL_HRAM_SIZE = 0x7F;

		constexpr u16 HRAM_START = 0xFF80;
		constexpr u16 HRAM_END = HRAM_START + TOTAL_HRAM_SIZE - 1;
		// ~HRAM
	}


	class RAM
	{

	public:

		RAM() = default;

	public:

		void WriteWRAM_Byte(u16 address, u8 value);
		u8 ReadWRAM_Byte(u16 address);

		void WriteVRAM_Byte(u16 address, u8 value);
		u8 ReadVRAM_Byte(u16 address);

		void WriteEXT_RAM_Byte(u16 address, u8 value);
		u8 ReadEXT_RAM_Byte(u16 address);

		void WriteHRAM_Byte(u16 address, u8 value);
		u8 ReadHRAM_Byte(u16 address);

	public:

		static bool IsWRAM_Addr(u16 address);

		static bool IsVRAM_Addr(u16 address);

		static bool IsHRAM_Addr(u16 address);

	private:

		using WRAM_BANK = std::array<u8, RAM_ADDR::WRAM_BANK_SIZE>;

		WRAM_BANK WRAM_Bank_0{};
		std::array<WRAM_BANK, 7> WRAM_Banks_1_7{};

		u8 currentWRAMBank = 0;

		using VRAM_BANK = std::array<u8, RAM_ADDR::VRAM_BANK_SIZE>;

		std::array<VRAM_BANK, 2> VRAM_Banks{};

		u8 currentVRAMBank = 0;

		using HRAM = std::array<u8, RAM_ADDR::TOTAL_HRAM_SIZE>;

		HRAM HRAM_Mem{};


	};
}