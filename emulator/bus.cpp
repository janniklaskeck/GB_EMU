#include "bus.h"
#include "cart.h"
#include "emu.h"
#include "ram.h"
#include "io.h"

#include <memory>
#include "joypad.h"
#include "timer.h"
#include "cpu.h"
#include "ppu.h"

using namespace GB;

u8 MEM_BUS::ReadByte(u16 address) const
{
	u8 readValue = 0;
	bool addressSupported = false;
	const char* addrType = "NoType";

	if (Cartridge::IsROM_Addr(address) || Cartridge::IsEXT_RAM_Addr(address))
	{
		readValue = EMU::GetCartridge()->ReadByte(address);
		addressSupported = true;
		addrType = "ROM";
	}

	if (RAM::IsWRAM_Addr(address))
	{
		readValue = EMU::GetRAM()->ReadWRAM_Byte(address);
		addressSupported = true;
		addrType = "WRAM";
	}
	
	if (RAM::IsVRAM_Addr(address))
	{
		readValue = EMU::GetRAM()->ReadVRAM_Byte(address);
		addressSupported = true;
		addrType = "VRAM";
	}

	if (RAM::IsHRAM_Addr(address))
	{
		readValue = EMU::GetRAM()->ReadHRAM_Byte(address);
		addressSupported = true;
		addrType = "HRAM";
	}

	if (IO::IsIO_Addr(address))
	{
		readValue = EMU::GetIO()->ReadByte(address);
		addressSupported = true;
		addrType = "IO";
	}

	if (PPU::IsOAM_Addr(address))
	{
		readValue = EMU::GetPPU()->ReadOAM_Byte(address);
		addressSupported = true;
		addrType = "OAM";
	}
	
	//printf("- BUS: R (%s) 0x%04X = 0x%02X\n", addrType, address, readValue);
	
	if (!addressSupported)
	{
		NO_IMPL("MemRead");
		return 0;
	}

	return readValue;
}

void MEM_BUS::WriteByte(u16 address, u8 value)
{
	bool addressSupported = false;
	const char* addrType = "NoType";

	if (Cartridge::IsROM_Addr(address) || Cartridge::IsEXT_RAM_Addr(address))
	{
		EMU::GetCartridge()->WriteByte(address, value);
		addressSupported = true;
		addrType = "ROM";
	}
	else if (RAM::IsWRAM_Addr(address))
	{
		EMU::GetRAM()->WriteWRAM_Byte(address, value);
		addressSupported = true;
		addrType = "WRAM";
	}
	else if (RAM::IsVRAM_Addr(address))
	{
		EMU::GetRAM()->WriteVRAM_Byte(address, value);
		addressSupported = true;
		addrType = "VRAM";
	}
	else if (RAM::IsHRAM_Addr(address))
	{
		EMU::GetRAM()->WriteHRAM_Byte(address, value);
		addressSupported = true;
		addrType = "HRAM";
	}
	else if (Joypad::IsJoypad_Addr(address))
	{
		addrType = "Joypad";
	} 
	else if (IO::IsIO_Addr(address))
	{
		EMU::GetIO()->WriteByte(address, value);
		addressSupported = true;
		addrType = "IO";
	}
	else if (address == 0xFFFF)
	{
		EMU::GetEMU()->GetCPU()->SetInterruptEnabledFlags(value);
		addressSupported = true;
	}

	//printf("- BUS: W (%s) 0x%04X = 0x%02X\n", addrType, address, value);
	
	if (!addressSupported)
	{
		NO_IMPL("MemWrite");
	}
}

void MEM_BUS::WriteWord(u16 address, u16 value)
{
	WriteByte(address, value & 0xFF);
	WriteByte(address + 1, (value & 0xFF00) >> 8);
}

void MEM_BUS::DMA_Start(u8 start)
{
	dmaTransferActive = true;
	dmaStartDelay = 2;
	dmaValue = start;
	dmaCurrentByte = 0;
}

void MEM_BUS::DMA_Tick()
{
	if (!dmaTransferActive)
	{
		return;
	}

	if (dmaStartDelay > 0)
	{
		dmaStartDelay--;
		return;
	}

	const u8 newValue = EMU::GetBUS()->ReadByte(dmaValue * 0x100) + dmaCurrentByte;
	EMU::GetPPU()->WriteOAM_Byte(dmaCurrentByte, newValue);
	dmaCurrentByte++;

	dmaTransferActive = dmaCurrentByte < TOTAL_OAM_SIZE;

	if (!dmaTransferActive)
	{
		EMU::GetCPU()->Sleep(2000);
	}
}

bool MEM_BUS::DMA_TransferActive() const
{
	return dmaTransferActive;
}
