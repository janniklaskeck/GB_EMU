#include <instructions.h>
#include <cpu.h>
#include <array>
#include <vector>
#include <iostream>
#include <iomanip>

using namespace GB;

std::vector<Instruction> instructions = 
{
	{IN_NOP, AM_IMP},
	{IN_LD, AM_R_D16, RT_BC},
	{IN_LD, AM_MR_R, RT_BC, RT_A},
	{IN_INC, AM_R, RT_BC},
	{IN_INC, AM_R, RT_B},
	{IN_DEC, AM_R, RT_B},
	{IN_LD, AM_R_D8, RT_B},
	{IN_RLCA},
	{IN_LD, AM_A16_R, RT_NONE, RT_SP},
	{IN_ADD, AM_R_R, RT_HL, RT_BC},
	{IN_LD, AM_R_MR, RT_A, RT_BC},
	{IN_DEC, AM_R, RT_BC},
	{IN_INC, AM_R, RT_C},
	{IN_DEC, AM_R, RT_C},
	{IN_LD, AM_R_D8, RT_C},
	{IN_RRCA},

	//0x1X
	{IN_STOP},
	{IN_LD, AM_R_D16, RT_DE},
	{IN_LD, AM_MR_R, RT_DE, RT_A},
	{IN_INC, AM_R, RT_DE},
	{IN_INC, AM_R, RT_D},
	{IN_DEC, AM_R, RT_D},
	{IN_LD, AM_R_D8, RT_D},
	{IN_RLA},
	{IN_JR, AM_D8},
	{IN_ADD, AM_R_R, RT_HL, RT_DE},
	{IN_LD, AM_R_MR, RT_A, RT_DE},
	{IN_DEC, AM_R, RT_DE},
	{IN_INC, AM_R, RT_E},
	{IN_DEC, AM_R, RT_E},
	{IN_LD, AM_R_D8, RT_E},
	{IN_RRA},

	//0x2X
	{IN_JR, AM_D8, RT_NONE, RT_NONE, CT_NZ},
	{IN_LD, AM_R_D16, RT_HL},
	{IN_LD, AM_HLI_R, RT_HL, RT_A},
	{IN_INC, AM_R, RT_HL},
	{IN_INC, AM_R, RT_H},
	{IN_DEC, AM_R, RT_H},
	{IN_LD, AM_R_D8, RT_H},
	{IN_DAA},
	{IN_JR, AM_D8, RT_NONE, RT_NONE, CT_Z},
	{IN_ADD, AM_R_R, RT_HL, RT_HL},
	{IN_LD, AM_R_HLI, RT_A, RT_HL},
	{IN_DEC, AM_R, RT_HL},
	{IN_INC, AM_R, RT_L},
	{IN_DEC, AM_R, RT_L},
	{IN_LD, AM_R_D8, RT_L},
	{IN_CPL},

	//0x3X
	{IN_JR, AM_D8, RT_NONE, RT_NONE, CT_NC},
	{IN_LD, AM_R_D16, RT_SP},
	{IN_LD, AM_HLD_R, RT_HL, RT_A},
	{IN_INC, AM_R, RT_SP},
	{IN_INC, AM_MR, RT_HL},
	{IN_DEC, AM_MR, RT_HL},
	{IN_LD, AM_MR_D8, RT_HL},
	{IN_SCF},
	{IN_JR, AM_D8, RT_NONE, RT_NONE, CT_C},
	{IN_ADD, AM_R_R, RT_HL, RT_SP},
	{IN_LD, AM_R_HLD, RT_A, RT_HL},
	{IN_DEC, AM_R, RT_SP},
	{IN_INC, AM_R, RT_A},
	{IN_DEC, AM_R, RT_A},
	{IN_LD, AM_R_D8, RT_A},
	{IN_CCF},

	//0x4X
	{IN_LD, AM_R_R, RT_B, RT_B},
	{IN_LD, AM_R_R, RT_B, RT_C},
	{IN_LD, AM_R_R, RT_B, RT_D},
	{IN_LD, AM_R_R, RT_B, RT_E},
	{IN_LD, AM_R_R, RT_B, RT_H},
	{IN_LD, AM_R_R, RT_B, RT_L},
	{IN_LD, AM_R_MR, RT_B, RT_HL},
	{IN_LD, AM_R_R, RT_B, RT_A},
	{IN_LD, AM_R_R, RT_C, RT_B},
	{IN_LD, AM_R_R, RT_C, RT_C},
	{IN_LD, AM_R_R, RT_C, RT_D},
	{IN_LD, AM_R_R, RT_C, RT_E},
	{IN_LD, AM_R_R, RT_C, RT_H},
	{IN_LD, AM_R_R, RT_C, RT_L},
	{IN_LD, AM_R_MR, RT_C, RT_HL},
	{IN_LD, AM_R_R, RT_C, RT_A},

	//0x5X
	{IN_LD, AM_R_R,  RT_D, RT_B},
	{IN_LD, AM_R_R,  RT_D, RT_C},
	{IN_LD, AM_R_R,  RT_D, RT_D},
	{IN_LD, AM_R_R,  RT_D, RT_E},
	{IN_LD, AM_R_R,  RT_D, RT_H},
	{IN_LD, AM_R_R,  RT_D, RT_L},
	{IN_LD, AM_R_MR, RT_D, RT_HL},
	{IN_LD, AM_R_R,  RT_D, RT_A},
	{IN_LD, AM_R_R,  RT_E, RT_B},
	{IN_LD, AM_R_R,  RT_E, RT_C},
	{IN_LD, AM_R_R,  RT_E, RT_D},
	{IN_LD, AM_R_R,  RT_E, RT_E},
	{IN_LD, AM_R_R,  RT_E, RT_H},
	{IN_LD, AM_R_R,  RT_E, RT_L},
	{IN_LD, AM_R_MR, RT_E, RT_HL},
	{IN_LD, AM_R_R,  RT_E, RT_A},

	//0x6X
	{ IN_LD, AM_R_R,  RT_H, RT_B },
	{ IN_LD, AM_R_R,  RT_H, RT_C },
	{ IN_LD, AM_R_R,  RT_H, RT_D },
	{ IN_LD, AM_R_R,  RT_H, RT_E },
	{ IN_LD, AM_R_R,  RT_H, RT_H },
	{ IN_LD, AM_R_R,  RT_H, RT_L },
	{ IN_LD, AM_R_MR, RT_H, RT_HL },
	{ IN_LD, AM_R_R,  RT_H, RT_A },
	{ IN_LD, AM_R_R,  RT_L, RT_B },
	{ IN_LD, AM_R_R,  RT_L, RT_C },
	{ IN_LD, AM_R_R,  RT_L, RT_D },
	{ IN_LD, AM_R_R,  RT_L, RT_E },
	{ IN_LD, AM_R_R,  RT_L, RT_H },
	{ IN_LD, AM_R_R,  RT_L, RT_L },
	{ IN_LD, AM_R_MR, RT_L, RT_HL },
	{ IN_LD, AM_R_R,  RT_L, RT_A },

	//0x7X
	{ IN_LD, AM_MR_R,  RT_HL, RT_B },
	{ IN_LD, AM_MR_R,  RT_HL, RT_C },
	{ IN_LD, AM_MR_R,  RT_HL, RT_D },
	{ IN_LD, AM_MR_R,  RT_HL, RT_E },
	{ IN_LD, AM_MR_R,  RT_HL, RT_H },
	{ IN_LD, AM_MR_R,  RT_HL, RT_L },
	{ IN_HALT },
	{ IN_LD, AM_MR_R,  RT_HL, RT_A },
	{ IN_LD, AM_R_R,  RT_A, RT_B },
	{ IN_LD, AM_R_R,  RT_A, RT_C },
	{ IN_LD, AM_R_R,  RT_A, RT_D },
	{ IN_LD, AM_R_R,  RT_A, RT_E },
	{ IN_LD, AM_R_R,  RT_A, RT_H },
	{ IN_LD, AM_R_R,  RT_A, RT_L },
	{ IN_LD, AM_R_MR, RT_A, RT_HL },
	{ IN_LD, AM_R_R,  RT_A, RT_A },

	//0x8X
	{IN_ADD, AM_R_R, RT_A, RT_B},
	{IN_ADD, AM_R_R, RT_A, RT_C},
	{IN_ADD, AM_R_R, RT_A, RT_D},
	{IN_ADD, AM_R_R, RT_A, RT_E},
	{IN_ADD, AM_R_R, RT_A, RT_H},
	{IN_ADD, AM_R_R, RT_A, RT_L},
	{IN_ADD, AM_R_MR, RT_A, RT_HL},
	{IN_ADD, AM_R_R, RT_A, RT_A},
	{IN_ADC, AM_R_R, RT_A, RT_B},
	{IN_ADC, AM_R_R, RT_A, RT_C},
	{IN_ADC, AM_R_R, RT_A, RT_D},
	{IN_ADC, AM_R_R, RT_A, RT_E},
	{IN_ADC, AM_R_R, RT_A, RT_H},
	{IN_ADC, AM_R_R, RT_A, RT_L},
	{IN_ADC, AM_R_MR, RT_A, RT_HL},
	{IN_ADC, AM_R_R, RT_A, RT_A},

	//0x9X
	{IN_SUB, AM_R_R, RT_A, RT_B},
	{IN_SUB, AM_R_R, RT_A, RT_C},
	{IN_SUB, AM_R_R, RT_A, RT_D},
	{IN_SUB, AM_R_R, RT_A, RT_E},
	{IN_SUB, AM_R_R, RT_A, RT_H},
	{IN_SUB, AM_R_R, RT_A, RT_L},
	{IN_SUB, AM_R_MR, RT_A, RT_HL},
	{IN_SUB, AM_R_R, RT_A, RT_A},
	{IN_SBC, AM_R_R, RT_A, RT_B},
	{IN_SBC, AM_R_R, RT_A, RT_C},
	{IN_SBC, AM_R_R, RT_A, RT_D},
	{IN_SBC, AM_R_R, RT_A, RT_E},
	{IN_SBC, AM_R_R, RT_A, RT_H},
	{IN_SBC, AM_R_R, RT_A, RT_L},
	{IN_SBC, AM_R_MR, RT_A, RT_HL},
	{IN_SBC, AM_R_R, RT_A, RT_A},


	//0xAX
	{IN_AND, AM_R_R, RT_A, RT_B},
	{IN_AND, AM_R_R, RT_A, RT_C},
	{IN_AND, AM_R_R, RT_A, RT_D},
	{IN_AND, AM_R_R, RT_A, RT_E},
	{IN_AND, AM_R_R, RT_A, RT_H},
	{IN_AND, AM_R_R, RT_A, RT_L},
	{IN_AND, AM_R_MR, RT_A, RT_HL},
	{IN_AND, AM_R_R, RT_A, RT_A},
	{IN_XOR, AM_R_R, RT_A, RT_B},
	{IN_XOR, AM_R_R, RT_A, RT_C},
	{IN_XOR, AM_R_R, RT_A, RT_D},
	{IN_XOR, AM_R_R, RT_A, RT_E},
	{IN_XOR, AM_R_R, RT_A, RT_H},
	{IN_XOR, AM_R_R, RT_A, RT_L},
	{IN_XOR, AM_R_MR, RT_A, RT_HL},
	{IN_XOR, AM_R_R, RT_A, RT_A},

	//0xBX
	{IN_OR, AM_R_R, RT_A, RT_B},
	{IN_OR, AM_R_R, RT_A, RT_C},
	{IN_OR, AM_R_R, RT_A, RT_D},
	{IN_OR, AM_R_R, RT_A, RT_E},
	{IN_OR, AM_R_R, RT_A, RT_H},
	{IN_OR, AM_R_R, RT_A, RT_L},
	{IN_OR, AM_R_MR, RT_A, RT_HL},
	{IN_OR, AM_R_R, RT_A, RT_A},
	{IN_CP, AM_R_R, RT_A, RT_B},
	{IN_CP, AM_R_R, RT_A, RT_C},
	{IN_CP, AM_R_R, RT_A, RT_D},
	{IN_CP, AM_R_R, RT_A, RT_E},
	{IN_CP, AM_R_R, RT_A, RT_H},
	{IN_CP, AM_R_R, RT_A, RT_L},
	{IN_CP, AM_R_MR, RT_A, RT_HL},
	{IN_CP, AM_R_R, RT_A, RT_A},

	//0xCX
	{ IN_RET, AM_IMP, RT_NONE, RT_NONE, CT_NZ },
	{ IN_POP, AM_R, RT_BC },
	{ IN_JP, AM_D16, RT_NONE, RT_NONE, CT_NZ },
	{ IN_JP, AM_D16 },
	{ IN_CALL, AM_D16, RT_NONE, RT_NONE, CT_NZ },
	{ IN_PUSH, AM_R, RT_BC },
	{ IN_ADD, AM_R_D8, RT_A },
	{ IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x00 },
	{ IN_RET, AM_IMP, RT_NONE, RT_NONE, CT_Z },
	{ IN_RET },
	{ IN_JP, AM_D16, RT_NONE, RT_NONE, CT_Z },
	{ IN_CB, AM_D8 },
	{ IN_CALL, AM_D16, RT_NONE, RT_NONE, CT_Z },
	{ IN_CALL, AM_D16 },
	{ IN_ADC, AM_R_D8, RT_A },
	{ IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x08 },

	//0xDX
	{IN_RET, AM_IMP, RT_NONE, RT_NONE, CT_NC},
	{IN_POP, AM_R, RT_DE},
	{IN_JP, AM_D16, RT_NONE, RT_NONE, CT_NC},
	{},
	{IN_CALL, AM_D16, RT_NONE, RT_NONE, CT_NC},
	{IN_PUSH, AM_R, RT_DE},
	{IN_SUB, AM_R_D8, RT_A},
	{IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x10},
	{IN_RET, AM_IMP, RT_NONE, RT_NONE, CT_C},
	{IN_RETI},
	{IN_JP, AM_D16, RT_NONE, RT_NONE, CT_C},
	{},
	{IN_CALL, AM_D16, RT_NONE, RT_NONE, CT_C},
	{},
	{IN_SBC, AM_R_D8, RT_A},
	{IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x18},

	//0xEX
	{IN_LDH, AM_A8_R, RT_NONE, RT_A},
	{IN_POP, AM_R, RT_HL},
	{IN_LD, AM_MR_R, RT_C, RT_A},
	{},
	{},
	{IN_PUSH, AM_R, RT_HL},
	{IN_AND, AM_R_D8, RT_A},
	{IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x20},
	{IN_ADD, AM_R_D8, RT_SP},
	{IN_JP, AM_R, RT_HL},
	{IN_LD, AM_A16_R, RT_NONE, RT_A},
	{},
	{},
	{},
	{IN_XOR, AM_R_D8, RT_A},
	{IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x28},


	//0xFX
	{IN_LDH, AM_R_A8, RT_A},
	{IN_POP, AM_R, RT_AF},
	{IN_LD, AM_R_MR, RT_A, RT_C},
	{IN_DI},
	{},
	{IN_PUSH, AM_R, RT_AF},
	{IN_OR, AM_R_D8, RT_A},
	{IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x30},
	{IN_LD, AM_HL_SPD, RT_HL, RT_SP},
	{IN_LD, AM_R_R, RT_SP, RT_HL},
	{IN_LD, AM_R_A16, RT_A},
	{IN_EI},
	{},
	{},
	{ IN_CP, AM_R_D8, RT_A },
	{ IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x38 },
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

	const u32 opcode = instruction->type;
	const std::string& nameString = instruction->GetInstructionName();

	const u32 addr_mode = instruction->mode;
	const std::string& addressModeString = instruction->GetAddrModeName();

	const u32 reg1 = instruction->reg_1;
	const std::string& reg1String = instruction->GetReg1Name();

	const u32 reg2 = instruction->reg_2;
	const std::string& reg2String = instruction->GetReg2Name();

	

	//std::cout << std::setw(32) << std::endl;
	std::printf("CPU: (%*s | %*s | %*s | %*s)\n",
				7, nameString.c_str(),
				6, addressModeString.c_str(),
				2, reg1String.c_str(),
				2, reg2String.c_str());
}

std::string GB::Instruction::GetInstructionName() const
{
	return instructionNames[type];
}

std::string Instruction::GetAddrModeName() const
{
	return addressModeNames[mode];
}

std::string Instruction::GetReg1Name() const
{
	return registerNames[reg_1];
}

std::string Instruction::GetReg2Name() const
{
	return registerNames[reg_2];
}
