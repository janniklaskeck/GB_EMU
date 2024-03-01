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

	if (Cartridge::IsROM_Addr(address) || Cartridge::IsEXT_RAM_Addr(address))
	{
		return EMU::GetCartridge()->ReadByte(address);
	}

	if (RAM::IsWRAM_Addr(address))
	{
		return EMU::GetRAM()->ReadWRAM_Byte(address);
	}

	if (RAM::IsVRAM_Addr(address))
	{
		return EMU::GetRAM()->ReadVRAM_Byte(address);
	}

	if (RAM::IsHRAM_Addr(address))
	{
		return EMU::GetRAM()->ReadHRAM_Byte(address);
	}

	if (IO::IsIO_Addr(address))
	{
		return EMU::GetIO()->ReadByte(address);
	}

	NO_IMPL("MemRead");
	return 0;
}

void MEM_BUS::WriteByte(u16 address, u8 value)
{
	if (Cartridge::IsROM_Addr(address) || Cartridge::IsEXT_RAM_Addr(address))
	{
		EMU::GetCartridge()->WriteByte(address, value);
		return;
	}

	if (RAM::IsWRAM_Addr(address))
	{
		EMU::GetRAM()->WriteWRAM_Byte(address, value);
		return;
	}

	if (RAM::IsVRAM_Addr(address))
	{
		EMU::GetRAM()->WriteVRAM_Byte(address, value);
		return;
	}

	if (RAM::IsHRAM_Addr(address))
	{
		EMU::GetRAM()->WriteHRAM_Byte(address, value);
		return;
	}

	if (IO::IsIO_Addr(address))
	{
		EMU::GetIO()->WriteByte(address, value);
		return;
	}

	NO_IMPL("MemWrite");
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

	const u8 newValue = EMU::GetBUS()->ReadByte((dmaValue * 0x100) + dmaCurrentByte);
	EMU::GetPPU()->WriteOAM_Byte(dmaCurrentByte, newValue);
	dmaCurrentByte++;

	dmaTransferActive = dmaCurrentByte < TOTAL_OAM_SIZE;
}

bool MEM_BUS::DMA_TransferActive() const
{
	return dmaTransferActive;
}
