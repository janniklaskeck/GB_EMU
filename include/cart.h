#pragma once

#include <common.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace GB
{
	namespace RAM_ADDR
	{
		// EXT RAM
		constexpr u16 EXT_RAM_BANK_SIZE = 0x2000;

		constexpr u16 EXT_RAM_BANK = 0xA000;

		constexpr u16 EXT_RAM_START = EXT_RAM_BANK;
		constexpr u16 EXT_RAM_END = EXT_RAM_START + EXT_RAM_BANK_SIZE - 1;
		// ~EXT RAM
	};

	struct ROM_Header
	{
		u8 entry[4];
		u8 logo[0x30];

		char title[16];
		u16 new_lic_code;
		u8 sgb_flag;
		u8 type;
		u8 rom_size;
		u8 ram_size;
		u8 dest_code;
		u8 lic_code;
		u8 version;
		u8 checksum;
		u16 global_checksum;
	};

	class Cartridge
	{
	public:

		Cartridge() = default;

	public:

		bool Load(char* CartridgeFilePath);

		u8 ReadByte(u16 address) const;

		void WriteByte(u16 address, u8 value);

		static bool IsROM_Addr(u16 address);

		static bool IsEXT_RAM_Addr(u16 address);

	private:

		const std::string& Get_Cartridge_License_Name() const;

		const std::string& Get_Cartridge_Type_Name() const;

	private:

		ROM_Header Header_ROM = {};

		char filename[1024] = {};
		u32 rom_size = 0;
		u8* rom_data = nullptr;
		ROM_Header* header = nullptr;

		static std::string INVALID_ROM_TYPE;
		static std::string INVALID_LICENSE_CODE;
	};
}
