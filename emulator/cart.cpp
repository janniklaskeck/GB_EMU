#include "cart.h"

#include <xutility>
#include <iterator>
#include <fstream>
#include <format>

using namespace GB;

void ROM_Header::PrintHeaderInfo()
{
	std::string cgbFlagString = "Not supported";
	if (GetCGBFlag() == CGB_Flag::CGB_Compatible)
	{
		cgbFlagString = "CGB Compatible";
	}
	else if (GetCGBFlag() == CGB_Flag::CGB_Only)
	{
		cgbFlagString = "CGB Only";
	}

	const std::string cartridgeTypeNameString = GetCartridgeTypeName();

	const std::string oldLicenseeCodeString = GetCartridgeLicenseName();
	const std::string newLicenseeCodeString = GetCartridgeLicenseName();

	const std::string destinationCodeString = destinationCode ? "Overseas only" : "Japan+Overseas";

	printf("\tTitle    : %s\n", title.data());
	printf("\tSGB Flag : %u\n", sgbFlag);
	printf("\tCGB      : %s\n", cgbFlagString.c_str());
	printf("\tType     : %2.2X (%s)\n", type, cartridgeTypeNameString.c_str());
	printf("\tROM Size : %u KiB\n", 32 << romSize);
	printf("\tRAM Size : %u KiB (%2.2X)\n", GetExtRamBankCount() * 8 * 1024, ramSize);
	printf("\tDst Code : %2.2X (%s)\n", destinationCode, destinationCodeString.c_str());
	printf("\tLIC Code : %2.2X (%s) (New : %2.2X (%s))\n", oldLicenseeCode, oldLicenseeCodeString.c_str(), newLicenseeCode, newLicenseeCodeString.c_str());
	printf("\tROM Vers : %2.2X\n", version);
	printf("\tChecksum : %2.2X (Global : %4.4X)\n", checksum, globalChecksum);
}

CGB_Flag ROM_Header::GetCGBFlag() const
{
	const u8 flag = (u8)title[15];

	if (flag == 0x80)
	{
		return CGB_Flag::CGB_Compatible;
	}

	if (flag == 0xC0)
	{
		return CGB_Flag::CGB_Only;
	}

	return CGB_Flag::NONE;
}

const std::string& ROM_Header::GetCartridgeLicenseName() const
{
	if (oldLicenseeCode == 0x33)
	{
		return NewLicenseCodes.at(newLicenseeCode);
	}

	if (oldLicenseeCode < OldLicenseCodes.size())
	{
		return OldLicenseCodes.at(oldLicenseeCode);
	}

	return INVALID_LICENSE_CODE;
}

const std::string& ROM_Header::GetCartridgeTypeName() const
{
	if (type < RomTypes.size())
	{
		return RomTypes.at(type);
	}

	return INVALID_ROM_TYPE;
}

u8 ROM_Header::GetExtRamBankCount() const
{
	switch (ramSize)
	{
	case 0x00:
	case 0x01: return 0;
	case 0x02: return 1;
	case 0x03: return 4;
	case 0x04: return 16;
	case 0x05: return 8;
	default: return 0;
	}
}

u8 ROM_Header::GetRomBankCount() const
{
	switch (romSize)
	{
	case 0x00: return 2;
	case 0x01: return 4;
	case 0x02: return 8;
	case 0x03: return 16;
	case 0x04: return 32;
	case 0x05: return 64;
	case 0x06: return 128;
	case 0x07: return 256;
	case 0x08: return 512;
	default: return 2;
	}
}

u32 ROM_Header::GetRomTotalSize() const
{
	return 32 * 1024 * (1 << romSize);
}

bool Cartridge::Load(std::filesystem::path cartridgeFilePath)
{
	std::ifstream fileStream;
	fileStream.open(cartridgeFilePath, std::ios::binary | std::ios::ate);

	if (!fileStream.good())
	{
		printf(std::format("Failed to open: {}\n", cartridgeFilePath.string()).c_str());
		return false;
	}

	printf(std::format("Opened: {}\n", cartridgeFilePath.string()).c_str());

	const u32 romSize = fileStream.tellg();
	romData.resize(romSize);

	fileStream.seekg(std::ios::beg);
	fileStream.read(romData.data(), romSize);

	fileStream.close();

	header = (ROM_Header*)(romData.data() + 0x100);
	//header->title[15] = 0;

	printf("Cartridge Loaded:\n");
	header->PrintHeaderInfo();

	const bool checksumValid = ValidateChecksum();
	printf("\tChecksum : %2.2X (%s)\n", header->checksum, checksumValid ? "PASSED" : "FAILED");

	if (checksumValid)
	{
		Init_ExtRam();
	}

	return checksumValid;
}

bool Cartridge::ValidateChecksum() const
{
	u16 x = 0;
	for (u16 i = 0x0134; i <= 0x014C; i++)
	{
		x = x - romData[i] - 1;
	}
	return x & 0xFF;
}

u8 Cartridge::ReadByte(u16 address) const
{
    if (IsEXT_RAM_Addr(address))
    {
		if (header->GetExtRamBankCount() == 0)
		{
			printf("Trying to read EXT RAM with no banks available!\n");
			__debugbreak();
			return 0;
		}

		return EXT_RAM_Banks[currentExtRamBank][address - RAM_ADDR::EXT_RAM_START];
    }

    return romData[address];
}

void Cartridge::WriteByte(u16 address, u8 value)
{
    if (IsEXT_RAM_Addr(address))
    {
		if (header->GetExtRamBankCount() == 0)
		{
			printf("Trying to write EXT RAM with no banks available!\n");
			__debugbreak();
			return;
		}

		EXT_RAM_Banks[currentExtRamBank][address - RAM_ADDR::EXT_RAM_START] = value;
        return;
    }

	/*if (address >= 0x0 && address <= 0x1FFF)
	{
		extRamEnabled = (value & 0xF) == 0xA;
		return;
	}

	if (address >= 0x2000 && address <= 0x3FFF)
	{

		return;
	}

	if (address >= 0x4000 && address <= 0x5FFF)
	{

		return;
	}

	if (address >= 0x6000 && address <= 0x7FFF)
	{

		return;
	}*/

    NO_IMPL("CartWrite");
}

bool Cartridge::IsROM_Addr(u16 address)
{
    return address <= 0x7FFF;
}

bool Cartridge::IsEXT_RAM_Addr(u16 address)
{
	return address >= RAM_ADDR::EXT_RAM_START && address <= RAM_ADDR::EXT_RAM_END;
}

void Cartridge::Init_ExtRam()
{

}
