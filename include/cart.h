#pragma once

#include "common.h"

#include <string>
#include <array>
#include <filesystem>

namespace GB
{
	namespace RAM_ADDR
	{
		// EXT RAM
		constexpr u16 EXT_RAM_BANK_SIZE = 0x2000;

		constexpr u16 EXT_RAM_START = 0xA000;
		constexpr u16 EXT_RAM_END = EXT_RAM_START + EXT_RAM_BANK_SIZE - 1;
		// ~EXT RAM
	};

	struct ROM_Header
	{
		std::array<u8, 4> EntryPoint;
		std::array<u8, 0x30> logoData;

		std::array<char, 16> title;
		u16 newLicenseeCode;
		u8 sgbFlag;
		u8 type;
		u8 romSize;
		u8 ramSize;
		u8 destinationCode;
		u8 oldLicenseeCode;
		u8 version;
		u8 checksum;
		u16 globalChecksum;

		void PrintHeaderInfo();

		CGB_Flag GetCGBFlag() const;

		u8 GetRomBankCount() const;

		u32 GetRomTotalSize() const;

		u8 GetExtRamBankCount() const;

		const std::string& GetCartridgeLicenseName() const;

		const std::string& GetCartridgeTypeName() const;
	};

	class Cartridge
	{
	public:

		Cartridge() = default;

	public:

		bool Load(std::filesystem::path cartridgeFilePath);

		u8 ReadByte(u16 address) const;

		void WriteByte(u16 address, u8 value);

		static bool IsROM_Addr(u16 address);

		static bool IsEXT_RAM_Addr(u16 address);

	private:

		bool ValidateChecksum() const;

		void Init_ExtRam();

	private:

		std::vector<char> romData;
		ROM_Header* header = nullptr;

		using EXT_RAM_BANK = std::array<u8, RAM_ADDR::EXT_RAM_BANK_SIZE>;

		bool extRamEnabled = false;
		u8 currentExtRamBank = 0;
		std::array<EXT_RAM_BANK, 16> EXT_RAM_Banks;

	};
}
