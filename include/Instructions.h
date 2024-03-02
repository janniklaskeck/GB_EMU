#pragma once

#include "common.h"
#include <string>

namespace GB
{
	struct Instruction
	{
		InstrType type;
		AddrMode mode;
		RegisterType reg_1;
		RegisterType reg_2;
		CondType cond;
		u8 param;

		static Instruction* GetInstruction(u8 opcode);

		std::string GetInstructionName() const;

		std::string GetAddrModeName() const;

		std::string GetReg1Name() const;
		std::string GetReg2Name() const;

		static void PrintInfo(Instruction* instruction);
	};

}
