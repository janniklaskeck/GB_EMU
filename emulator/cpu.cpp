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

//#define DEBUG_PRINT_LOG
#define DEBUG_PRINT_SCREEN
//#define GB_DOCTOR

#ifdef GB_DOCTOR
std::ofstream gbdFile("gbd.log");
#endif

CPU::CPU()
{
	registers = {};
	registers.PC = 0x100;
	registers.SP = 0xFFFE;
	registers.Write(RegisterType::AF, 0x01B0);
	registers.Write(RegisterType::BC, 0x0013);
	registers.Write(RegisterType::DE, 0x00D8);
	registers.Write(RegisterType::HL, 0x014d);

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

#ifdef DEBUG_PRINT

		if (instruction)
		{
			//Instruction::PrintInfo(instruction);
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

		if (EMU::GetEMU()->GetCycles() % 100000 == 0)
		{
			printf(debug.c_str());
		}

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
												 EMU::GetBUS()->ReadByte(PC + 1),
												 EMU::GetBUS()->ReadByte(PC + 2),
												 EMU::GetBUS()->ReadByte(PC + 3)
		);
		gbdFile << gbDoctor;
#endif

#endif

#ifdef DEBUG_PRINT_SCREEN
		EMU::GetEMU()->DebugUpdate();
		EMU::GetEMU()->DebugPrint();
#endif

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
	if (!instruction)
	{
		return;
	}

	switch (instruction->type)
	{
	case InstrType::NOP: CPU::Instruction_NOP(); return;
	case InstrType::LD: CPU::Instruction_LD(); return;
	case InstrType::LDH: CPU::Instruction_LDH(); return;
	case InstrType::INC: CPU::Instruction_INC(); return;
	case InstrType::DEC: CPU::Instruction_DEC(); return;
	case InstrType::RLCA: CPU::Instruction_RLCA(); return;
	case InstrType::ADD: CPU::Instruction_ADD(); return;
	case InstrType::ADC: CPU::Instruction_ADC(); return;
	case InstrType::SUB: CPU::Instruction_SUB(); return;
	case InstrType::SBC: CPU::Instruction_SBC(); return;
	case InstrType::RRCA: CPU::Instruction_RRCA(); return;
	case InstrType::CALL:
	case InstrType::RST: CPU::Instruction_CALL_RST(); return;
	case InstrType::JR:
	case InstrType::JP: CPU::Instruction_JP_JR(); return;
	case InstrType::RETI:
	case InstrType::RET: CPU::Instruction_RET_RETI(); return;
	case InstrType::DI: CPU::Instruction_EI_DI(); return;
	case InstrType::EI: CPU::Instruction_EI_DI(); return;
	case InstrType::CB: CPU::Instruction_CB(); return;
	case InstrType::AND:
	case InstrType::XOR:
	case InstrType::OR: CPU::Instruction_AND_OR_XOR(); return;
	case InstrType::CP: CPU::Instruction_CP(); return;
	case InstrType::POP:
	case InstrType::PUSH: CPU::Instruction_PUSH_POP(); return;
	case InstrType::STOP: CPU::Instruction_STOP(); return;
	case InstrType::HALT: CPU::Instruction_HALT(); return;
	case InstrType::RLA:
	case InstrType::RRA: CPU::Instruction_RLA_RRA(); return;
	case InstrType::DAA: CPU::Instruction_DAA(); return;
	case InstrType::CPL: CPU::Instruction_CPL(); return;
	case InstrType::CCF:
	case InstrType::SCF: CPU::Instruction_SCF_CCF(); return;
	default:
		break;
	}
}

u8 CPU::ReadBytePC()
{
	const u8 value = EMU::GetBUS()->ReadByte(registers.PC);
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

bool CPU::IsInterruptSet(const IntType type) const
{
	return IF_Flags & (u8)type;
}

u8 CPU::GetInterruptEnabledFlags() const
{
	return IE_Flags;
}

void CPU::SetInterruptEnabledFlags(u8 newFlags)
{
	IE_Flags = newFlags;
}

bool CPU::IsInterruptEnabled(const IntType type) const
{
	return IE_Flags & (u8)type;
}

void CPU::HandleInterrupts()
{
	auto executeInterrupt = [this](u16 address, IntType type)
	{
		if (IsInterruptSet(type) && IsInterruptEnabled(type))
		{
			Stack_PushWord(registers.GetPC());
			registers.SetPC(address);

			IF_Flags &= ~(u8)type;
			halted = false;
			interruptsEnabled = false;

			return true;
		}

		return false;
	};

	if (executeInterrupt(0x40, IntType::IT_VBlank))
	{
		return;
	}

	if (executeInterrupt(0x48, IntType::IT_LCD_Stat))
	{
		return;
	}

	if (executeInterrupt(0x50, IntType::IT_Timer))
	{
		return;
	}

	if (executeInterrupt(0x58, IntType::IT_Serial))
	{
		return;
	}

	if (executeInterrupt(0x60, IntType::IT_Joypad))
	{
		return;
	}
}

void CPU::RequestInterrupt(const IntType type)
{
	const u8 currentFlags = GetInterruptFlags();

	SetInterruptFlags(currentFlags | (u8)type);
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
	case AddrMode::R_D16:
	case AddrMode::R_A16:
	{
		paramString = std::format("{},{:#04X}", reg1Name.c_str(), fetched_data);
		break;
	}
	case AddrMode::R:
	{
		paramString = std::format("{}", reg1Name.c_str());
		break;
	}
	case AddrMode::R_R:
	{
		paramString = std::format("{},{}", reg1Name.c_str(), reg2Name.c_str());
		break;
	}
	case AddrMode::MR_R:
	{
		paramString = std::format("({}),{}", reg1Name.c_str(), reg2Name.c_str());
		break;
	}
	case AddrMode::MR:
	{
		paramString = std::format("({})", reg1Name.c_str());
		break;
	}
	case AddrMode::R_MR:
	{
		paramString = std::format("{},({})", reg1Name.c_str(), reg2Name.c_str());
		break;
	}
	case AddrMode::R_D8:
	case AddrMode::R_A8:
	{
		paramString = std::format("{},{:#02X}", reg1Name.c_str(), fetched_data & 0xFF);
		break;
	}
	case AddrMode::R_HLI:
	{
		paramString = std::format("{},({}+)", reg1Name.c_str(), reg2Name.c_str());
		break;
	}
	case AddrMode::R_HLD:
	{
		paramString = std::format("{},({}-)", reg1Name.c_str(), reg2Name.c_str());
		break;
	}
	case AddrMode::HLI_R:
	{
		paramString = std::format("({}),{}", reg1Name.c_str(), reg2Name.c_str());
		break;
	}
	case AddrMode::HLD_R:
	{
		paramString = std::format("({}-),{}", reg1Name.c_str(), reg2Name.c_str());
		break;
	}
	case AddrMode::A8_R:
	{
		const u8 value = EMU::GetBUS()->ReadByte(registers.PC - 1);
		paramString = std::format("{:#02X},{}", value, reg2Name.c_str());
		break;
	}
	case AddrMode::HL_SPD:
	{
		paramString = std::format("({}),SP+{}", reg1Name.c_str(), fetched_data & 0xFF);
		break;
	}
	case AddrMode::D8:
	{
		paramString = std::format("{:#02X}", fetched_data & 0xFF);
		break;
	}
	case AddrMode::D16:
	{
		paramString = std::format("{:#04X}", fetched_data);
		break;
	}
	case AddrMode::MR_D8:
	{
		paramString = std::format("({}),{:#02X}", reg1Name.c_str(), fetched_data & 0xFF);
		break;
	}
	case AddrMode::A16_R:
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
