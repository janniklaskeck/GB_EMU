#include "cpu.h"
#include "Instructions.h"
#include "emu.h"

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
	case AM_IMP:
	{
		return;
	}
	case AM_R_R:
	{
		fetched_data = registers.Read(instruction->reg_2);
		return;
	}
	case AM_R:
	{
		fetched_data = registers.Read(instruction->reg_1);
		return;
	}
	case AM_R_HLI:
	case AM_R_HLD:
	{
		const u16 HLValue = registers.Read(RT_HL);
		fetched_data = ReadByteBUS(HLValue);
		EMU::Cycle(1);
		return;
	}
	case AM_HLI_R:
	case AM_HLD_R:
	{
		const u16 HLValue = registers.Read(RT_HL);
		SetMemDest(HLValue);

		fetched_data = registers.Read(instruction->reg_2);
		return;
	}
	case AM_A16_R:
	case AM_D16_R:
	{
		SetMemDest(ReadWordPC());
		EMU::Cycle(1);

		fetched_data = registers.Read(instruction->reg_2);
		return;
	}
	case AM_A8_R:
	{
		SetMemDest(ReadBytePC() | 0xFF00);
		EMU::Cycle(1);

		fetched_data = registers.Read(instruction->reg_2);
		return;
	}
	case AM_R_A8:
	{
		fetched_data = ReadBytePC();
		EMU::Cycle(1);
		return;
	}
	case AM_MR_R:
	{
		fetched_data = registers.Read(instruction->reg_2);
		const u16 regValue = registers.Read(instruction->reg_1);

		if (instruction->reg_1 == RT_C)
		{
			SetMemDest(regValue | 0xFF00);
		}
		else
		{
			SetMemDest(regValue);
		}
		return;
	}
	case AM_D8:
	case AM_R_D8:
	{
		fetched_data = ReadBytePC();
		EMU::Cycle(1);
		return;
	}
	case AM_R_MR:
	{
		u16 address = registers.Read(instruction->reg_2);

		if (instruction->reg_2 == RT_C)
		{
			address |= 0xFF00;
		}
		fetched_data = ReadByteBUS(address);
		EMU::Cycle(1);
		return;
	}
	case AM_HL_SPD:
	{
		fetched_data = ReadBytePC();
		EMU::Cycle(1);
		return;
	}
	case AM_R_D16:
	case AM_D16:
	{
		fetched_data = ReadWordPC();
		EMU::Cycle(2);
		return;
	}
	case AM_R_A16:
	{
		const u16 address = ReadWordPC();
		EMU::Cycle(2);
		fetched_data = ReadByteBUS(address);
		EMU::Cycle(1);
		return;
	}
	case AM_MR_D8:
	{
		fetched_data = ReadBytePC();
		EMU::Cycle(1);

		SetMemDest(registers.Read(instruction->reg_1));
		return;
	}
	case AM_MR:
	{
		SetMemDest(registers.Read(instruction->reg_1));
		fetched_data = ReadByteBUS(mem_dest);
		EMU::Cycle(1);
		return;
	}
	default:
		break;
	}

	exit(-7);
}
