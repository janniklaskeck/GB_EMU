#include <instructions.h>
#include <cpu.h>
#include <array>
#include <vector>
#include <iostream>
#include <iomanip>

using namespace GB;

std::vector<Instruction> instructions =
{
	{ InstrType::NOP,	AddrMode::IMP},
	{ InstrType::LD,		AddrMode::R_D16,	RegisterType::BC},
	{ InstrType::LD,		AddrMode::MR_R,	RegisterType::BC,	RegisterType::A},
	{ InstrType::INC,	AddrMode::R,		RegisterType::BC},
	{ InstrType::INC,	AddrMode::R,		RegisterType::B},
	{ InstrType::DEC,	AddrMode::R,		RegisterType::B},
	{ InstrType::LD,		AddrMode::R_D8,	RegisterType::B},
	{ InstrType::RLCA},
	{ InstrType::LD,		AddrMode::A16_R,	RegisterType::NONE,	RegisterType::SP},
	{ InstrType::ADD,	AddrMode::R_R,	RegisterType::HL,	RegisterType::BC},
	{ InstrType::LD,		AddrMode::R_MR,	RegisterType::A,		RegisterType::BC},
	{ InstrType::DEC,	AddrMode::R,		RegisterType::BC},
	{ InstrType::INC,	AddrMode::R,		RegisterType::C},
	{ InstrType::DEC,	AddrMode::R,		RegisterType::C},
	{ InstrType::LD,		AddrMode::R_D8,	RegisterType::C},
	{ InstrType::RRCA},

	//0x1X
	{ InstrType::STOP},
	{ InstrType::LD,		AddrMode::R_D16,	RegisterType::DE},
	{ InstrType::LD,		AddrMode::MR_R,	RegisterType::DE,	RegisterType::A},
	{ InstrType::INC,	AddrMode::R,		RegisterType::DE},
	{ InstrType::INC,	AddrMode::R,		RegisterType::D},
	{ InstrType::DEC,	AddrMode::R,		RegisterType::D},
	{ InstrType::LD,		AddrMode::R_D8,	RegisterType::D},
	{ InstrType::RLA},
	{ InstrType::JR,		AddrMode::D8},
	{ InstrType::ADD,	AddrMode::R_R,	RegisterType::HL,	RegisterType::DE},
	{ InstrType::LD,		AddrMode::R_MR,	RegisterType::A,		RegisterType::DE},
	{ InstrType::DEC,	AddrMode::R,		RegisterType::DE},
	{ InstrType::INC,	AddrMode::R,		RegisterType::E},
	{ InstrType::DEC,	AddrMode::R,		RegisterType::E},
	{ InstrType::LD,		AddrMode::R_D8,	RegisterType::E},
	{ InstrType::RRA},

	//0x2X
	{ InstrType::JR,		AddrMode::D8,	RegisterType::NONE,	RegisterType::NONE, CondType::NZ},
	{ InstrType::LD,		AddrMode::R_D16,	RegisterType::HL},
	{ InstrType::LD,		AddrMode::HLI_R,	RegisterType::HL,	RegisterType::A},
	{ InstrType::INC,	AddrMode::R,		RegisterType::HL},
	{ InstrType::INC,	AddrMode::R,		RegisterType::H},
	{ InstrType::DEC,	AddrMode::R,		RegisterType::H},
	{ InstrType::LD,		AddrMode::R_D8,	RegisterType::H},
	{ InstrType::DAA},
	{ InstrType::JR,		AddrMode::D8,	RegisterType::NONE,	RegisterType::NONE, CondType::Z},
	{ InstrType::ADD,	AddrMode::R_R,	RegisterType::HL,	RegisterType::HL},
	{ InstrType::LD,		AddrMode::R_HLI,	RegisterType::A,		RegisterType::HL},
	{ InstrType::DEC,	AddrMode::R,		RegisterType::HL},
	{ InstrType::INC,	AddrMode::R,		RegisterType::L},
	{ InstrType::DEC,	AddrMode::R,		RegisterType::L},
	{ InstrType::LD,		AddrMode::R_D8,	RegisterType::L},
	{ InstrType::CPL},

	//0x3X
	{ InstrType::JR,		AddrMode::D8,	RegisterType::NONE,	RegisterType::NONE, CondType::NC},
	{ InstrType::LD,		AddrMode::R_D16,	RegisterType::SP},
	{ InstrType::LD,		AddrMode::HLD_R,	RegisterType::HL,	RegisterType::A},
	{ InstrType::INC,	AddrMode::R,		RegisterType::SP},
	{ InstrType::INC,	AddrMode::MR,	RegisterType::HL},
	{ InstrType::DEC,	AddrMode::MR,	RegisterType::HL},
	{ InstrType::LD,		AddrMode::MR_D8,	RegisterType::HL},
	{ InstrType::SCF},
	{ InstrType::JR,		AddrMode::D8,	RegisterType::NONE,	RegisterType::NONE, CondType::C},
	{ InstrType::ADD,	AddrMode::R_R,	RegisterType::HL,	RegisterType::SP},
	{ InstrType::LD,		AddrMode::R_HLD,	RegisterType::A,		RegisterType::HL},
	{ InstrType::DEC,	AddrMode::R,		RegisterType::SP},
	{ InstrType::INC,	AddrMode::R,		RegisterType::A},
	{ InstrType::DEC,	AddrMode::R,		RegisterType::A},
	{ InstrType::LD,		AddrMode::R_D8,	RegisterType::A},
	{ InstrType::CCF},

	//0x4X
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::B,		RegisterType::B},
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::B,		RegisterType::C},
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::B,		RegisterType::D},
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::B,		RegisterType::E},
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::B,		RegisterType::H},
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::B,		RegisterType::L},
	{ InstrType::LD,		AddrMode::R_MR,	RegisterType::B,		RegisterType::HL},
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::B,		RegisterType::A},
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::C,		RegisterType::B},
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::C,		RegisterType::C},
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::C,		RegisterType::D},
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::C,		RegisterType::E},
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::C,		RegisterType::H},
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::C,		RegisterType::L},
	{ InstrType::LD,		AddrMode::R_MR,	RegisterType::C,		RegisterType::HL},
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::C,		RegisterType::A},

	//0x5X
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::D,		RegisterType::B},
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::D,		RegisterType::C},
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::D,		RegisterType::D},
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::D,		RegisterType::E},
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::D,		RegisterType::H},
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::D,		RegisterType::L},
	{ InstrType::LD,		AddrMode::R_MR,	RegisterType::D,		RegisterType::HL},
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::D,		RegisterType::A},
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::E,		RegisterType::B},
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::E,		RegisterType::C},
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::E,		RegisterType::D},
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::E,		RegisterType::E},
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::E,		RegisterType::H},
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::E,		RegisterType::L},
	{ InstrType::LD,		AddrMode::R_MR,	RegisterType::E,		RegisterType::HL},
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::E,		RegisterType::A},

	//0x6X
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::H,		RegisterType::B },
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::H,		RegisterType::C },
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::H,		RegisterType::D },
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::H,		RegisterType::E },
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::H,		RegisterType::H },
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::H,		RegisterType::L },
	{ InstrType::LD,		AddrMode::R_MR,	RegisterType::H,		RegisterType::HL },
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::H,		RegisterType::A },
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::L,		RegisterType::B },
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::L,		RegisterType::C },
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::L,		RegisterType::D },
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::L,		RegisterType::E },
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::L,		RegisterType::H },
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::L,		RegisterType::L },
	{ InstrType::LD,		AddrMode::R_MR,	RegisterType::L,		RegisterType::HL },
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::L,		RegisterType::A },

	//0x7X
	{ InstrType::LD,		AddrMode::MR_R,	RegisterType::HL,	RegisterType::B },
	{ InstrType::LD,		AddrMode::MR_R,	RegisterType::HL,	RegisterType::C },
	{ InstrType::LD,		AddrMode::MR_R,	RegisterType::HL,	RegisterType::D },
	{ InstrType::LD,		AddrMode::MR_R,	RegisterType::HL,	RegisterType::E },
	{ InstrType::LD,		AddrMode::MR_R,	RegisterType::HL,	RegisterType::H },
	{ InstrType::LD,		AddrMode::MR_R,	RegisterType::HL,	RegisterType::L },
	{ InstrType::HALT },
	{ InstrType::LD,		AddrMode::MR_R,	RegisterType::HL,	RegisterType::A },
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::A,		RegisterType::B },
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::A,		RegisterType::C },
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::A,		RegisterType::D },
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::A,		RegisterType::E },
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::A,		RegisterType::H },
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::A,		RegisterType::L },
	{ InstrType::LD,		AddrMode::R_MR,	RegisterType::A,		RegisterType::HL },
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::A,		RegisterType::A },

	//0x8X
	{ InstrType::ADD,	AddrMode::R_R,	RegisterType::A,		RegisterType::B },
	{ InstrType::ADD,	AddrMode::R_R,	RegisterType::A,		RegisterType::C },
	{ InstrType::ADD,	AddrMode::R_R,	RegisterType::A,		RegisterType::D },
	{ InstrType::ADD,	AddrMode::R_R,	RegisterType::A,		RegisterType::E },
	{ InstrType::ADD,	AddrMode::R_R,	RegisterType::A,		RegisterType::H },
	{ InstrType::ADD,	AddrMode::R_R,	RegisterType::A,		RegisterType::L },
	{ InstrType::ADD,	AddrMode::R_MR,	RegisterType::A,		RegisterType::HL },
	{ InstrType::ADD,	AddrMode::R_R,	RegisterType::A,		RegisterType::A },
	{ InstrType::ADC,	AddrMode::R_R,	RegisterType::A,		RegisterType::B },
	{ InstrType::ADC,	AddrMode::R_R,	RegisterType::A,		RegisterType::C },
	{ InstrType::ADC,	AddrMode::R_R,	RegisterType::A,		RegisterType::D },
	{ InstrType::ADC,	AddrMode::R_R,	RegisterType::A,		RegisterType::E },
	{ InstrType::ADC,	AddrMode::R_R,	RegisterType::A,		RegisterType::H },
	{ InstrType::ADC,	AddrMode::R_R,	RegisterType::A,		RegisterType::L },
	{ InstrType::ADC,	AddrMode::R_MR,	RegisterType::A,		RegisterType::HL },
	{ InstrType::ADC,	AddrMode::R_R,	RegisterType::A,		RegisterType::A },

	//0x9X
	{ InstrType::SUB,	AddrMode::R_R,	RegisterType::A,		RegisterType::B },
	{ InstrType::SUB,	AddrMode::R_R,	RegisterType::A,		RegisterType::C },
	{ InstrType::SUB,	AddrMode::R_R,	RegisterType::A,		RegisterType::D },
	{ InstrType::SUB,	AddrMode::R_R,	RegisterType::A,		RegisterType::E },
	{ InstrType::SUB,	AddrMode::R_R,	RegisterType::A,		RegisterType::H },
	{ InstrType::SUB,	AddrMode::R_R,	RegisterType::A,		RegisterType::L },
	{ InstrType::SUB,	AddrMode::R_MR,	RegisterType::A,		RegisterType::HL },
	{ InstrType::SUB,	AddrMode::R_R,	RegisterType::A,		RegisterType::A },
	{ InstrType::SBC,	AddrMode::R_R,	RegisterType::A,		RegisterType::B },
	{ InstrType::SBC,	AddrMode::R_R,	RegisterType::A,		RegisterType::C },
	{ InstrType::SBC,	AddrMode::R_R,	RegisterType::A,		RegisterType::D },
	{ InstrType::SBC,	AddrMode::R_R,	RegisterType::A,		RegisterType::E },
	{ InstrType::SBC,	AddrMode::R_R,	RegisterType::A,		RegisterType::H },
	{ InstrType::SBC,	AddrMode::R_R,	RegisterType::A,		RegisterType::L },
	{ InstrType::SBC,	AddrMode::R_MR,	RegisterType::A,		RegisterType::HL },
	{ InstrType::SBC,	AddrMode::R_R,	RegisterType::A,		RegisterType::A },


	//0xAX
	{ InstrType::AND,	AddrMode::R_R,	RegisterType::A,		RegisterType::B },
	{ InstrType::AND,	AddrMode::R_R,	RegisterType::A,		RegisterType::C },
	{ InstrType::AND,	AddrMode::R_R,	RegisterType::A,		RegisterType::D },
	{ InstrType::AND,	AddrMode::R_R,	RegisterType::A,		RegisterType::E },
	{ InstrType::AND,	AddrMode::R_R,	RegisterType::A,		RegisterType::H },
	{ InstrType::AND,	AddrMode::R_R,	RegisterType::A,		RegisterType::L },
	{ InstrType::AND,	AddrMode::R_MR,	RegisterType::A,		RegisterType::HL },
	{ InstrType::AND,	AddrMode::R_R,	RegisterType::A,		RegisterType::A },
	{ InstrType::XOR,	AddrMode::R_R,	RegisterType::A,		RegisterType::B },
	{ InstrType::XOR,	AddrMode::R_R,	RegisterType::A,		RegisterType::C },
	{ InstrType::XOR,	AddrMode::R_R,	RegisterType::A,		RegisterType::D },
	{ InstrType::XOR,	AddrMode::R_R,	RegisterType::A,		RegisterType::E },
	{ InstrType::XOR,	AddrMode::R_R,	RegisterType::A,		RegisterType::H },
	{ InstrType::XOR,	AddrMode::R_R,	RegisterType::A,		RegisterType::L },
	{ InstrType::XOR,	AddrMode::R_MR,	RegisterType::A,		RegisterType::HL },
	{ InstrType::XOR,	AddrMode::R_R,	RegisterType::A,		RegisterType::A },

	//0xBX
	{ InstrType::OR,		AddrMode::R_R,	RegisterType::A,		RegisterType::B },
	{ InstrType::OR,		AddrMode::R_R,	RegisterType::A,		RegisterType::C },
	{ InstrType::OR,		AddrMode::R_R,	RegisterType::A,		RegisterType::D },
	{ InstrType::OR,		AddrMode::R_R,	RegisterType::A,		RegisterType::E },
	{ InstrType::OR,		AddrMode::R_R,	RegisterType::A,		RegisterType::H },
	{ InstrType::OR,		AddrMode::R_R,	RegisterType::A,		RegisterType::L },
	{ InstrType::OR,		AddrMode::R_MR,	RegisterType::A,		RegisterType::HL },
	{ InstrType::OR,		AddrMode::R_R,	RegisterType::A,		RegisterType::A },
	{ InstrType::CP,		AddrMode::R_R,	RegisterType::A,		RegisterType::B },
	{ InstrType::CP,		AddrMode::R_R,	RegisterType::A,		RegisterType::C },
	{ InstrType::CP,		AddrMode::R_R,	RegisterType::A,		RegisterType::D },
	{ InstrType::CP,		AddrMode::R_R,	RegisterType::A,		RegisterType::E },
	{ InstrType::CP,		AddrMode::R_R,	RegisterType::A,		RegisterType::H },
	{ InstrType::CP,		AddrMode::R_R,	RegisterType::A,		RegisterType::L },
	{ InstrType::CP,		AddrMode::R_MR,	RegisterType::A,		RegisterType::HL },
	{ InstrType::CP,		AddrMode::R_R,	RegisterType::A,		RegisterType::A },

	//0xCX
	{ InstrType::RET,	AddrMode::IMP,	RegisterType::NONE,	RegisterType::NONE, CondType::NZ },
	{ InstrType::POP,	AddrMode::R,		RegisterType::BC },
	{ InstrType::JP,		AddrMode::D16,	RegisterType::NONE,	RegisterType::NONE, CondType::NZ },
	{ InstrType::JP,		AddrMode::D16 },
	{ InstrType::CALL,	AddrMode::D16,	RegisterType::NONE,	RegisterType::NONE, CondType::NZ },
	{ InstrType::PUSH,	AddrMode::R,		RegisterType::BC },
	{ InstrType::ADD,	AddrMode::R_D8,	RegisterType::A },
	{ InstrType::RST,	AddrMode::IMP,	RegisterType::NONE,	RegisterType::NONE, CondType::NONE, 0x00 },
	{ InstrType::RET,	AddrMode::IMP,	RegisterType::NONE,	RegisterType::NONE, CondType::Z },
	{ InstrType::RET },
	{ InstrType::JP,		AddrMode::D16,	RegisterType::NONE,	RegisterType::NONE, CondType::Z },
	{ InstrType::CB,		AddrMode::D8 },
	{ InstrType::CALL,	AddrMode::D16,	RegisterType::NONE,	RegisterType::NONE, CondType::Z },
	{ InstrType::CALL,	AddrMode::D16 },
	{ InstrType::ADC,	AddrMode::R_D8,	RegisterType::A },
	{ InstrType::RST,	AddrMode::IMP,	RegisterType::NONE,	RegisterType::NONE, CondType::NONE, 0x08 },

	//0xDX
	{ InstrType::RET,	AddrMode::IMP,	RegisterType::NONE,	RegisterType::NONE, CondType::NC},
	{ InstrType::POP,	AddrMode::R,		RegisterType::DE},
	{ InstrType::JP,		AddrMode::D16,	RegisterType::NONE,	RegisterType::NONE, CondType::NC},
	{},
	{ InstrType::CALL,	AddrMode::D16,	RegisterType::NONE,	RegisterType::NONE, CondType::NC},
	{ InstrType::PUSH,	AddrMode::R,		RegisterType::DE},
	{ InstrType::SUB,	AddrMode::R_D8,	RegisterType::A},
	{ InstrType::RST,	AddrMode::IMP,	RegisterType::NONE,	RegisterType::NONE, CondType::NONE, 0x10},
	{ InstrType::RET,	AddrMode::IMP,	RegisterType::NONE,	RegisterType::NONE, CondType::C},
	{ InstrType::RETI},
	{ InstrType::JP,		AddrMode::D16,	RegisterType::NONE,	RegisterType::NONE, CondType::C},
	{},
	{ InstrType::CALL,	AddrMode::D16,	RegisterType::NONE,	RegisterType::NONE, CondType::C},
	{},
	{ InstrType::SBC,	AddrMode::R_D8,	RegisterType::A},
	{ InstrType::RST,	AddrMode::IMP,	RegisterType::NONE,	RegisterType::NONE, CondType::NONE, 0x18},

	//0xEX
	{ InstrType::LDH,	AddrMode::A8_R,	RegisterType::NONE,	RegisterType::A},
	{ InstrType::POP,	AddrMode::R,		RegisterType::HL},
	{ InstrType::LD,		AddrMode::MR_R,	RegisterType::C,		RegisterType::A},
	{},
	{},
	{ InstrType::PUSH,	AddrMode::R,		RegisterType::HL},
	{ InstrType::AND,	AddrMode::R_D8,	RegisterType::A},
	{ InstrType::RST,	AddrMode::IMP,	RegisterType::NONE,	RegisterType::NONE, CondType::NONE, 0x20},
	{ InstrType::ADD,	AddrMode::R_D8,	RegisterType::SP},
	{ InstrType::JP,		AddrMode::R,		RegisterType::HL},
	{ InstrType::LD,		AddrMode::A16_R,	RegisterType::NONE,	RegisterType::A},
	{},
	{},
	{},
	{ InstrType::XOR,	AddrMode::R_D8,	RegisterType::A},
	{ InstrType::RST,	AddrMode::IMP,	RegisterType::NONE,	RegisterType::NONE, CondType::NONE, 0x28},


	//0xFX
	{ InstrType::LDH,	AddrMode::R_A8,	RegisterType::A },
	{ InstrType::POP,	AddrMode::R,		RegisterType::AF },
	{ InstrType::LD,		AddrMode::R_MR,	RegisterType::A,		RegisterType::C },
	{ InstrType::DI },
	{},
	{ InstrType::PUSH,	AddrMode::R,		RegisterType::AF },
	{ InstrType::OR,		AddrMode::R_D8,	RegisterType::A },
	{ InstrType::RST,	AddrMode::IMP,	RegisterType::NONE,	RegisterType::NONE, CondType::NONE, 0x30 },
	{ InstrType::LD,		AddrMode::HL_SPD,RegisterType::HL,	RegisterType::SP },
	{ InstrType::LD,		AddrMode::R_R,	RegisterType::SP,	RegisterType::HL },
	{ InstrType::LD,		AddrMode::R_A16,	RegisterType::A },
	{ InstrType::EI },
	{},
	{},
	{ InstrType::CP,		AddrMode::R_D8,	RegisterType::A },
	{ InstrType::RST,	AddrMode::IMP,	RegisterType::NONE,	RegisterType::NONE, CondType::NONE, 0x38 },
};

std::vector<std::string> instructionNames =
{
	"NOP",
	"LD",
	"INC",
	"DEC",
	"RLCA",
	"ADD",
	"RRCA",
	"STOP",
	"RLA",
	"JR",
	"RRA",
	"DAA",
	"CPL",
	"SCF",
	"CCF",
	"HALT",
	"ADC",
	"SUB",
	"SBC",
	"AND",
	"XOR",
	"OR",
	"CP",
	"POP",
	"JP",
	"PUSH",
	"RET",
	"CB",
	"CALL",
	"RETI",
	"LDH",
	"DI",
	"EI",
	"RST",
	"IN_ERR",
	"IN_RLC",
	"IN_RRC",
	"IN_RL",
	"IN_RR",
	"IN_SLA",
	"IN_SRA",
	"IN_SWAP",
	"IN_SRL",
	"IN_BIT",
	"IN_RES",
	"IN_SET"
};

std::vector<std::string> addressModeNames =
{
	"IMP",
	"R_D16",
	"R_R",
	"MR_R",
	"R",
	"R_D8",
	"R_MR",
	"R_HLI",
	"R_HLD",
	"HLI_R",
	"HLD_R",
	"R_A8",
	"A8_R",
	"HL_SPD",
	"D16",
	"D8",
	"D16_R",
	"MR_D8",
	"MR",
	"A16_R",
	"R_A16"
};

std::vector<std::string> registerNames =
{
	"<>",
	"A",
	"F",
	"B",
	"C",
	"D",
	"E",
	"H",
	"L",
	"AF",
	"BC",
	"DE",
	"HL",
	"SP",
	"PC"
};

Instruction* Instruction::GetInstruction(u8 opcode)
{
	if (opcode < 0 || opcode >= instructions.size())
	{
		return nullptr;
	}

	return &instructions[opcode];
}

void Instruction::PrintInfo(Instruction* instruction)
{
	if (!instruction)
	{
		printf("Instruction is null!");
		return;
	}

	const std::string& nameString = instruction->GetInstructionName();
	const std::string& addressModeString = instruction->GetAddrModeName();
	const std::string& reg1String = instruction->GetReg1Name();
	const std::string& reg2String = instruction->GetReg2Name();

	std::printf("CPU: (%*s | %*s | %*s | %*s)\n",
				7, nameString.c_str(),
				6, addressModeString.c_str(),
				2, reg1String.c_str(),
				2, reg2String.c_str());
}

std::string GB::Instruction::GetInstructionName() const
{
	return instructionNames[(u32)type];
}

std::string Instruction::GetAddrModeName() const
{
	return addressModeNames[(u32)mode];
}

std::string Instruction::GetReg1Name() const
{
	return registerNames[(u32)reg_1];
}

std::string Instruction::GetReg2Name() const
{
	return registerNames[(u32)reg_2];
}
