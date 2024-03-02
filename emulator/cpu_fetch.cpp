#include "cpu.h"
#include "Instructions.h"
#include "emu.h"
#include "bus.h"

using namespace GB;

void CPU::FetchInstruction()
{
	opcode = ReadBytePC();
	instruction = Instruction::GetInstruction(opcode);
}

void CPU::FetchData()
{
	mem_dest = 0;
	mem_dest_isMem = false;
	fetched_data = 0;

	if (!instruction)
	{
		return;
	}

	switch (instruction->mode)
	{
	case AddrMode::IMP:
	{
		return;
	}
	case AddrMode::R_R:
	{
		fetched_data = registers.Read(instruction->reg_2);
		return;
	}
	case AddrMode::R:
	{
		fetched_data = registers.Read(instruction->reg_1);
		return;
	}
	case AddrMode::R_HLI:
	case AddrMode::R_HLD:
	{
		const u16 HLValue = registers.Read(RegisterType::HL);
		fetched_data = EMU::GetBUS()->ReadByte(HLValue);
		EMU::Cycle(1);
		return;
	}
	case AddrMode::HLI_R:
	case AddrMode::HLD_R:
	{
		const u16 HLValue = registers.Read(RegisterType::HL);
		SetMemDest(HLValue);

		fetched_data = registers.Read(instruction->reg_2);
		return;
	}
	case AddrMode::A16_R:
	case AddrMode::D16_R:
	{
		SetMemDest(ReadWordPC());
		EMU::Cycle(1);

		fetched_data = registers.Read(instruction->reg_2);
		return;
	}
	case AddrMode::A8_R:
	{
		SetMemDest(ReadBytePC() | 0xFF00);
		EMU::Cycle(1);

		fetched_data = registers.Read(instruction->reg_2);
		return;
	}
	case AddrMode::R_A8:
	{
		fetched_data = ReadBytePC();
		EMU::Cycle(1);
		return;
	}
	case AddrMode::MR_R:
	{
		fetched_data = registers.Read(instruction->reg_2);
		const u16 regValue = registers.Read(instruction->reg_1);

		if (instruction->reg_1 == RegisterType::C)
		{
			SetMemDest(regValue | 0xFF00);
		}
		else
		{
			SetMemDest(regValue);
		}
		return;
	}
	case AddrMode::D8:
	case AddrMode::R_D8:
	{
		fetched_data = ReadBytePC();
		EMU::Cycle(1);
		return;
	}
	case AddrMode::R_MR:
	{
		u16 address = registers.Read(instruction->reg_2);

		if (instruction->reg_2 == RegisterType::C)
		{
			address |= 0xFF00;
		}
		fetched_data = EMU::GetBUS()->ReadByte(address);
		EMU::Cycle(1);
		return;
	}
	case AddrMode::HL_SPD:
	{
		fetched_data = ReadBytePC();
		EMU::Cycle(1);
		return;
	}
	case AddrMode::R_D16:
	case AddrMode::D16:
	{
		fetched_data = ReadWordPC();
		EMU::Cycle(2);
		return;
	}
	case AddrMode::R_A16:
	{
		const u16 address = ReadWordPC();
		EMU::Cycle(2);
		fetched_data = EMU::GetBUS()->ReadByte(address);
		EMU::Cycle(1);
		return;
	}
	case AddrMode::MR_D8:
	{
		fetched_data = ReadBytePC();
		EMU::Cycle(1);

		SetMemDest(registers.Read(instruction->reg_1));
		return;
	}
	case AddrMode::MR:
	{
		SetMemDest(registers.Read(instruction->reg_1));
		fetched_data = EMU::GetBUS()->ReadByte(mem_dest);
		EMU::Cycle(1);
		return;
	}
	default:
		break;
	}

	exit(-7);
}
