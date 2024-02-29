#include <cpu.h>
#include "bus.h"
#include "Instructions.h"
#include "emu.h"
#include "timer.h"

#include <thread>
#include <chrono>
#include <format>
#include <fstream>

using namespace GB;

//#define GB_DOCTOR

#ifdef GB_DOCTOR
std::ofstream gbdFile("gbd.log");
#endif

CPU::CPU()
{
	registers = {};
	registers.PC = 0x100;
	registers.SP = 0xFFFE;
	registers.Write(RT_AF, 0x01B0);
	registers.Write(RT_BC, 0x0013);
	registers.Write(RT_DE, 0x00D8);
	registers.Write(RT_HL, 0x014d);

	//EMU::GetTimer()->WriteByte(0xFF04, 0xABCC);
}

bool CPU::Step()
{
	if (!halted)
	{
		u16 PC = registers.PC;

		FetchInstruction();

		u16 PC_temp = registers.PC;

		FetchData();

		if (instruction)
		{
			//Instruction::PrintInfo(instruction);
			if (!GetFunction(instruction))
			{
				NO_IMPL("function");
			}
		}

		const std::string debug = std::format("{:08X} - {:04X} {:12} ({:02X} {:02X} {:02X}) A: {:02X} F: {} BC: {:02X}{:02X} DE: {:02X}{:02X} HL: {:02X}{:02X}\n",
											  EMU::GetEMU()->GetCycles(),
											  PC,
											  GetInstructionDebugString().c_str(),
											  opcode,
											  EMU::GetBUS()->ReadByte(PC + 1),
											  EMU::GetBUS()->ReadByte(PC + 2),
											  registers.A, registers.GetFlagsString().c_str(),
											  registers.B, registers.C,
											  registers.D, registers.E,
											  registers.H, registers.L);

		//if (EMU::GetEMU()->GetCycles() % 100000 == 0)
			printf(debug.c_str());

#ifdef GB_DOCTOR

		const std::string gbDoctor = std::format("A:{:02X} F:{:02X} B:{:02X} C:{:02X} D:{:02X} E:{:02X} H:{:02X} L:{:02X} SP:{:04X} PC:{:04X} PCMEM:{:02X},{:02X},{:02X},{:02X}\n",
											  registers.A,
											  registers.F,
											  registers.B,
											  registers.C,
											  registers.D,
											  registers.E,
											  registers.H,
											  registers.L,
											  registers.SP,
											  PC,
											  EMU::GetBUS()->ReadByte(PC),
											  EMU::GetBUS()->ReadByte(PC+1),
											  EMU::GetBUS()->ReadByte(PC+2),
											  EMU::GetBUS()->ReadByte(PC+3)
		);
		gbdFile << gbDoctor;
#endif

		EMU::GetEMU()->DebugUpdate();
		EMU::GetEMU()->DebugPrint();

		Execute();
	}
	else
	{
		EMU::GetEMU()->Cycle(1);

		if (IF_Flags)
		{
			halted = false;
		}
	}

	if (interruptsEnabled)
	{
		HandleInterrupts();
		enableInterrupts = false;
	}

	if (enableInterrupts)
	{
		interruptsEnabled = true;
	}

	return true;
}

void CPU::Execute()
{
	Instr_Function function = GetFunction(instruction);
	if (function)
	{
		function(*this, instruction);
	}
}

u8 CPU::ReadByteBUS(u16 address)
{
	const u8 value = EMU::GetBUS()->ReadByte(address);
	return value;
}

u8 CPU::ReadBytePC()
{
	const u8 value = ReadByteBUS(registers.PC);
	registers.PC++;
	return value;
}

u16 CPU::ReadWordPC()
{
	const u16 lo = ReadBytePC();
	const u16 hi = ReadBytePC();

	const u16 word = lo | (hi << 8);

	return word;
}

void CPU::SetMemDest(u16 address)
{
	mem_dest = address;
	mem_dest_isMem = true;
}

u8 CPU::GetInterruptFlags() const
{
	return IF_Flags;
}

void CPU::SetInterruptFlags(u8 newFlags)
{
	IF_Flags = newFlags;
}

bool CPU::IsInterruptSet(const InterruptType type) const
{
	return IF_Flags & type;
}

u8 CPU::GetInterruptEnabledFlags() const
{
	return IE_Flags;
}

void CPU::SetInterruptEnabledFlags(u8 newFlags)
{
	IE_Flags = newFlags;
}

bool CPU::IsInterruptEnabled(const InterruptType type) const
{
	return IE_Flags & type;
}

void CPU::HandleInterrupts()
{
	auto executeInterrupt = [this](u16 address, InterruptType type)
	{
		if (IsInterruptSet(type) && IsInterruptEnabled(type))
		{
			Stack_PushWord(registers.Read(RT_PC));
			registers.Write(RT_PC, address);

			IF_Flags &= ~type;
			halted = false;
			interruptsEnabled = false;

			return true;
		}

		return false;
	};

	if (executeInterrupt(0x40, IT_VBlank))
	{
		return;
	}

	if (executeInterrupt(0x48, IT_LCD_Stat))
	{
		return;
	}

	if (executeInterrupt(0x50, IT_Timer))
	{
		return;
	}

	if (executeInterrupt(0x58, IT_Serial))
	{
		return;
	}

	if (executeInterrupt(0x60, IT_Joypad))
	{
		return;
	}
}

void CPU::RequestInterrupt(const InterruptType type)
{
	const u8 currentFlags = GetInterruptFlags();

	SetInterruptFlags(currentFlags | type);
}

void CPU::Sleep(u32 ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

std::string CPU::GetInstructionDebugString() const
{
	std::string debugString = instruction->GetInstructionName() + " ";

	std::string paramString;

	const std::string reg1Name = instruction->GetReg1Name();
	const std::string reg2Name = instruction->GetReg2Name();

	switch (instruction->mode)
	{
	case AM_R_D16:
	case AM_R_A16:
	{
		paramString = std::format("{},{:#04X}", reg1Name.c_str(), fetched_data);
		break;
	}
	case AM_R:
	{
		paramString = std::format("{}", reg1Name.c_str());
		break;
	}
	case AM_R_R:
	{
		paramString = std::format("{},{}", reg1Name.c_str(), reg2Name.c_str());
		break;
	}
	case AM_MR_R:
	{
		paramString = std::format("({}),{}", reg1Name.c_str(), reg2Name.c_str());
		break;
	}
	case AM_MR:
	{
		paramString = std::format("({})", reg1Name.c_str());
		break;
	}
	case AM_R_MR:
	{
		paramString = std::format("{},({})", reg1Name.c_str(), reg2Name.c_str());
		break;
	}
	case AM_R_D8:
	case AM_R_A8:
	{
		paramString = std::format("{},{:#02X}", reg1Name.c_str(), fetched_data & 0xFF);
		break;
	}
	case AM_R_HLI:
	{
		paramString = std::format("{},({}+)", reg1Name.c_str(), reg2Name.c_str());
		break;
	}
	case AM_R_HLD:
	{
		paramString = std::format("{},({}-)", reg1Name.c_str(), reg2Name.c_str());
		break;
	}
	case AM_HLI_R:
	{
		paramString = std::format("({}),{}", reg1Name.c_str(), reg2Name.c_str());
		break;
	}
	case AM_HLD_R:
	{
		paramString = std::format("({}-),{}", reg1Name.c_str(), reg2Name.c_str());
		break;
	}
	case AM_A8_R:
	{
		const u8 value = EMU::GetBUS()->ReadByte(registers.PC - 1);
		paramString = std::format("{:#02X},{}", value, reg2Name.c_str());
		break;
	}
	case AM_HL_SPD:
	{
		paramString = std::format("({}),SP+{}", reg1Name.c_str(), fetched_data & 0xFF);
		break;
	}
	case AM_D8:
	{
		paramString = std::format("{:#02X}", fetched_data & 0xFF);
		break;
	}
	case AM_D16:
	{
		paramString = std::format("{:#04X}", fetched_data);
		break;
	}
	case AM_MR_D8:
	{
		paramString = std::format("({}),{:#02X}", reg1Name.c_str(), fetched_data & 0xFF);
		break;
	}
	case AM_A16_R:
	{
		paramString = std::format("({:#04X}),{}", fetched_data, reg2Name.c_str());
		break;
	}
	default:
		break;
	}

	debugString += paramString;
	return debugString;
}
