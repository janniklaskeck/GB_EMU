#include <cpu.h>
#include "bus.h"
#include "Instructions.h"
#include "emu.h"

using namespace GB;

void CPU::Stack_PushByte(u8 value)
{
	registers.Decrement(RT_SP);
	const u16 SP_Value = registers.Read(RT_SP);
	EMU::GetBUS()->WriteByte(SP_Value, value);
}

void CPU::Stack_PushWord(u16 value)
{
	const u8 hi = (value >> 8) & 0xFF;
	Stack_PushByte(hi);
	const u8 lo = value & 0xFF;
	Stack_PushByte(lo);
	stackDebug.push_back(value);
	stackCounter++;
	//printf("-- STA: PUSH %u(0x%04X) | SP: 0x%04X C: %u\n", value, value, registers.Read(RT_SP), stackCounter);
}

u8 CPU::Stack_PopByte()
{
	const u16 SP_Value = registers.Read(RT_SP);
	const u8 value = EMU::GetBUS()->ReadByte(SP_Value);
	registers.Increment(RT_SP);

	return value;
}

u16 CPU::Stack_PopWord()
{
	const u8 low = Stack_PopByte();
	const u8 hi = Stack_PopByte();
	const u16 value = (hi << 8) | low;
	stackDebug.erase(stackDebug.end() - 1);
	stackCounter--;
	//printf("-- STA: POP %u(0x%04X) | SP: 0x%04X C: %u\n", value, value, registers.Read(RT_SP), stackCounter);

	return value;
}

bool CPU::IsConditionMet(const cond_type condition) const
{
	
	switch (condition)
	{
	case CT_NZ: return !registers.GetZeroFlag();
	case CT_Z: return registers.GetZeroFlag();
	case CT_NC: return !registers.GetCarryFlag();
	case CT_C: return registers.GetCarryFlag();
	case CT_NONE:
	default: return true;
	}
}

Instr_Function CPU::GetFunction(const Instruction* instruction) const
{
	if (!instruction)
	{
		return nullptr;
	}

	switch (instruction->type)
	{
	case IN_NOP: return &CPU::Instruction_NOP;
	case IN_LD: return &CPU::Instruction_LD;
	case IN_LDH: return &CPU::Instruction_LDH;
	case IN_INC: return &CPU::Instruction_INC;
	case IN_DEC: return &CPU::Instruction_DEC;
	case IN_RLCA: return &CPU::Instruction_RLCA;
	case IN_ADD: return &CPU::Instruction_ADD;
	case IN_ADC: return &CPU::Instruction_ADC;
	case IN_SUB: return &CPU::Instruction_SUB;
	case IN_SBC: return &CPU::Instruction_SBC;
	case IN_RRCA: return &CPU::Instruction_RRCA;
	case IN_CALL:
	case IN_RST: return &CPU::Instruction_CALL_RST;
	case IN_JR:
	case IN_JP: return &CPU::Instruction_JP_JR;
	case IN_RETI:
	case IN_RET: return &CPU::Instruction_RET_RETI;
	case IN_DI: return &CPU::Instruction_EI_DI;
	case IN_EI: return &CPU::Instruction_EI_DI;
	case IN_CB: return &CPU::Instruction_CB;
	case IN_AND:
	case IN_XOR:
	case IN_OR: return &CPU::Instruction_AND_OR_XOR;
	case IN_CP: return &CPU::Instruction_CP;
	case IN_POP:
	case IN_PUSH: return &CPU::Instruction_PUSH_POP;
	case IN_STOP: return &CPU::Instruction_STOP;
	case IN_HALT: return &CPU::Instruction_HALT;
	case IN_RLA:
	case IN_RRA: return &CPU::Instruction_RLA_RRA;
	case IN_DAA: return &CPU::Instruction_DAA;
	case IN_CPL: return &CPU::Instruction_CPL;
	case IN_CCF:
	case IN_SCF: return &CPU::Instruction_SCF_CCF;
	default:
		break;

	}
	
	return nullptr;
}

void CPU::Instruction_NOP(const Instruction* instruction)
{
	EMU::Cycle(1);
}

void CPU::Instruction_LD(const Instruction* instruction)
{
	EMU::Cycle(1);

	if (instruction->mode == AM_R_HLI || instruction->mode == AM_HLI_R)
	{
		registers.Increment(RT_HL);
	}
	else if (instruction->mode == AM_R_HLD || instruction->mode == AM_HLD_R)
	{
		registers.Decrement(RT_HL);
	}

	if (mem_dest_isMem)
	{
		EMU::Cycle(1);

		if (Registers::IsWordSize(instruction->reg_2))
		{
			EMU::GetBUS()->WriteWord(mem_dest, fetched_data);
			EMU::Cycle(1);
		}
		else
		{
			EMU::GetBUS()->WriteByte(mem_dest, fetched_data);
		}

		return;
	}

	if (instruction->mode == AM_HL_SPD)
	{
		EMU::Cycle(1);

		const u16 HLValue = registers.Read(instruction->reg_2);
		const i8 signedOffset = fetched_data;
		const u8 hFlag = (HLValue & 0xF) + (signedOffset & 0xF) >= 0x10;
		const u8 cFlag = (HLValue & 0xFF) + (signedOffset & 0xFF) >= 0x100;

		registers.SetFlags(0, 0, hFlag, cFlag);
		registers.Write(instruction->reg_1, HLValue + signedOffset);

		return;
	}

	registers.Write(instruction->reg_1, fetched_data);
}

void CPU::Instruction_LDH(const Instruction* instruction)
{
	EMU::Cycle(1);

	if (mem_dest_isMem)
	{
		EMU::GetBUS()->WriteByte(mem_dest, fetched_data);
		return;
	}

	const u16 targetAddress = 0xFF00 | fetched_data;
	fetched_data = EMU::GetBUS()->ReadByte(targetAddress);

	registers.Write(instruction->reg_1, fetched_data);
}

void CPU::Instruction_INC(const Instruction* instruction)
{
	EMU::Cycle(1);

	const bool isWordReg = registers.IsWordSize(instruction->reg_1);
	const u16 newValue = fetched_data + 1;

	if (isWordReg)
	{
		EMU::Cycle(1);
	}

	if (mem_dest_isMem)
	{
		EMU::GetBUS()->WriteByte(mem_dest, newValue & 0xFF);
		EMU::Cycle(1);
	}
	else
	{
		registers.Write(instruction->reg_1, newValue & 0xFFFF);
	}

	if (isWordReg && instruction->mode != AM_MR)
	{
		return;
	}

	const u8 zFlag = (newValue & 0xFF) == 0;
	const u8 hFlag = ((fetched_data & 0xF) + 1) > 0xF;
	registers.SetFlags(zFlag, 0, hFlag, -1);
}

void CPU::Instruction_DEC(const Instruction* instruction)
{
	EMU::Cycle(1);

	const bool isWordReg = registers.IsWordSize(instruction->reg_1);
	u16 newValue = fetched_data - 1;

	if (isWordReg)
	{
		EMU::Cycle(1);
	}

	if (mem_dest_isMem)
	{
		EMU::GetBUS()->WriteByte(mem_dest, newValue & 0xFF);
		EMU::Cycle(1);
	}
	else
	{
		registers.Write(instruction->reg_1, newValue & 0xFFFF);
	}

	if (isWordReg && instruction->mode != AM_MR)
	{
		return;
	}

	const u8 zFlag = (newValue & 0xFF) == 0;
	const u8 hFlag = (((i16)fetched_data & 0xF) - 1) < 0;
	registers.SetFlags(zFlag, 1, hFlag, -1);
}

void CPU::Instruction_ADD(const Instruction* instruction)
{
	EMU::Cycle(1);

	const u16 currentValue = registers.Read(instruction->reg_1);
	u16 newValue = currentValue + fetched_data;

	u8 isZero = (newValue & 0xFF) == 0;
	bool hFlag = ((currentValue & 0xF) + (fetched_data & 0xF)) > 0xF;
	bool cFlag = ((i16)(currentValue & 0xFF) + (i16)(fetched_data & 0xFF)) > 0xFF;

	if (registers.IsWordSize(instruction->reg_1))
	{
		EMU::Cycle(1);

		if (instruction->reg_1 == RT_SP)
		{
			EMU::Cycle(1);
			newValue = currentValue + (i8)fetched_data;

			isZero = 0;
		}
		else
		{
			isZero = -1;
			hFlag = ((currentValue & 0xFFF) + (fetched_data & 0xFFF)) > 0xFFF;
			cFlag = ((u32)currentValue + (u32)fetched_data) > 0xFFFF;
		}
	}

	registers.Write(instruction->reg_1, newValue & 0xFFFF);
	registers.SetFlags(isZero, 0, hFlag, cFlag);
}

void CPU::Instruction_ADC(const Instruction* instruction)
{
	EMU::Cycle(1);

	const u16 currentValue = registers.Read(instruction->reg_1);
	const u16 data = fetched_data;
	const u16 carry = registers.GetCarryFlag();
	const u16 newValue = currentValue + data + carry;

	registers.Write(instruction->reg_1, newValue & 0xFFFF);

	u8 isZero = (newValue & 0xFF) == 0;
	u8 hFlag = ((currentValue & 0xF) + (fetched_data & 0xF) + carry) > 0xF;
	u8 cFlag = ((i16)(currentValue & 0xFF) + (i16)(fetched_data & 0xFF) + carry) > 0xFF;

	registers.SetFlags(isZero, 0, hFlag, cFlag);
}

void CPU::Instruction_SUB(const Instruction* instruction)
{
	EMU::Cycle(1);

	const u16 currentValue = registers.Read(instruction->reg_1);
	u32 newValue = currentValue - fetched_data;
	
	if (instruction->type == IN_SBC)
	{
		newValue -= registers.GetCarryFlag();
	}

	registers.Write(instruction->reg_1, newValue & 0xFFFF);

	bool isZero = (newValue & 0xFF) == 0;
	bool hFlag = (((i16)currentValue & 0xF) - ((i16)fetched_data & 0xF)) < 0;
	bool cFlag = (((i16)currentValue & 0xFF) - ((i16)fetched_data & 0xFF)) < 0;

	if (registers.IsWordSize(instruction->reg_1))
	{
		EMU::Cycle(1);
		hFlag = ((currentValue & 0xFFF) + (fetched_data & 0xFFF)) > 0xFFF;
		cFlag = newValue > 0xFFFF;
	}

	registers.SetFlags(isZero, 1, hFlag, cFlag);
}

void CPU::Instruction_SBC(const Instruction* instruction)
{
	EMU::Cycle(1);

	const u16 currentValue = registers.Read(instruction->reg_1);
	const u16 data = fetched_data;
	const u16 carry = registers.GetCarryFlag();
	const u16 newValue = currentValue - fetched_data - carry;

	registers.Write(instruction->reg_1, newValue & 0xFFFF);

	const u8 isZero = (newValue & 0xFF) == 0;
	const u8 hFlag = (((i16)currentValue & 0xF) - ((i16)fetched_data & 0xF) - carry) < 0;
	const u8 cFlag = (((i16)currentValue & 0xFF) - ((i16)fetched_data & 0xFF) - carry) < 0;

	registers.SetFlags(isZero, 1, hFlag, cFlag);
}

void CPU::Instruction_RLCA(const Instruction* instruction)
{
	EMU::Cycle(1);

	const u8 currentValue = registers.Read(RT_A);
	const u8 bitZero = (currentValue & 0x80) > 0;
	const u8 newValue = (currentValue << 1) | bitZero;

	registers.Write(RT_A, newValue);
	registers.SetFlags(0, 0, 0, bitZero);
}

void CPU::Instruction_RRCA(const Instruction* instruction)
{
	EMU::Cycle(1);

	const u8 currentValue = registers.Read(RT_A);
	const u8 bitZero = currentValue & 0x1;
	u8 newValue = currentValue >> 1;
	newValue |= bitZero << 7;

	registers.Write(RT_A, newValue);
	registers.SetFlags(0, 0, 0, bitZero);
}

void CPU::Instruction_JP_JR(const Instruction* instruction)
{
	EMU::Cycle(1);

	if (instruction->mode == AM_R)
	{
		const u16 jumpAddress = registers.Read(instruction->reg_1);
		registers.Write(RT_PC, jumpAddress);
		return;
	}
	
	if (instruction->mode == AM_D8)
	{
		const u16 reg_PC = registers.Read(RT_PC);
		const i8 signedOffset = fetched_data;
		fetched_data = reg_PC + signedOffset;
	}

	const u16 jumpAddress = fetched_data;

	if (IsConditionMet(instruction->cond))
	{
		registers.Write(RT_PC, jumpAddress);

		EMU::Cycle(1);
	}
}

void CPU::Instruction_CALL_RST(const Instruction* instruction)
{
	EMU::Cycle(1);

	if (instruction->type == IN_RST)
	{
		fetched_data = instruction->param;
	}

	const u16 jumpAddress = fetched_data;

	if (IsConditionMet(instruction->cond))
	{
		const u16 nextInstruction = registers.Read(RT_PC);
		Stack_PushWord(nextInstruction);
		registers.Write(RT_PC, jumpAddress);

		EMU::Cycle(3);
	}
}

void CPU::Instruction_RET_RETI(const Instruction* instruction)
{
	EMU::Cycle(2);

	if (instruction->type == IN_RETI)
	{
		enableInterrupts = true;
	}

	if (IsConditionMet(instruction->cond))
	{
		const u16 jumpAddress = Stack_PopWord();
		EMU::Cycle(2);

		registers.Write(RT_PC, jumpAddress);

		if (instruction->cond != CT_NONE)
		{
			EMU::Cycle(1);
		}
	}
}

void CPU::Instruction_EI_DI(const Instruction* instruction)
{
	EMU::Cycle(1);

	if (instruction->type == IN_EI)
	{
		enableInterrupts = true;
		return;
	}

	enableInterrupts = false;
	interruptsEnabled = false;
}

void CPU::Instruction_CB(const Instruction* instruction)
{
	EMU::Cycle(2);

	const u8 CB_opcode = (fetched_data >> 3);
	const u8 bitIndex = (fetched_data >> 3) & 0b111;

	reg_type rt_lookup[] = {
	RT_B,
	RT_C,
	RT_D,
	RT_E,
	RT_H,
	RT_L,
	RT_HL,
	RT_A
	};

	const reg_type reg = rt_lookup[fetched_data & 0b111];
	in_type CB_Type;
	if (CB_opcode < 8)
	{
		CB_Type = (in_type)((u16)in_type::IN_RLC + CB_opcode);
	}
	else
	{
		CB_Type = (in_type)((u16)in_type::IN_BIT + (CB_opcode >> 3) - 1);
	}

	const u16 regValue = registers.Read(reg);
	const u8 currentValue = reg == RT_HL ? EMU::GetBUS()->ReadByte(regValue) : regValue;
	u8 newValue = 0;
	switch (CB_Type)
	{
	case IN_RLC:
	{
		const u8 shiftedBit = (currentValue & 0x80) > 0;
		newValue = (currentValue << 1) | shiftedBit;

		registers.SetFlags(newValue == 0, 0, 0, shiftedBit);
		break;
	}
	case IN_RRC:
	{
		const u8 shiftedBit = (currentValue & 0x1);
		newValue = (currentValue >> 1) | (shiftedBit << 7);

		registers.SetFlags(newValue == 0, 0, 0, shiftedBit);
		break;
	}
	case IN_RL:
	{
		const u8 carryFlag = registers.GetCarryFlag();
		const u8 shiftedBit = (currentValue & 0x80) > 0;
		newValue = (currentValue << 1) | carryFlag;

		registers.SetFlags(newValue == 0, 0, 0, shiftedBit);
		break;
	}
	case IN_RR:
	{
		const u8 carryFlag = registers.GetCarryFlag();
		const u8 shiftedBit = (currentValue & 0x1);
		newValue = (currentValue >> 1) | (carryFlag << 7);

		registers.SetFlags(newValue == 0, 0, 0, shiftedBit);
		break;
	}
	case IN_SLA:
	{
		const u8 shiftedBit = (currentValue & 0x80) > 0;
		newValue = (currentValue << 1);

		registers.SetFlags(newValue == 0, 0, 0, shiftedBit);
		break;
	}
	case IN_SRA:
	{
		const u8 MSB = (currentValue & 0x80);
		const u8 shiftedBit = (currentValue & 0x1);
		newValue = (currentValue >> 1) | MSB;

		registers.SetFlags(newValue == 0, 0, 0, shiftedBit);
		break;
	}
	case IN_SWAP:
	{
		newValue = ((currentValue & 0x0F) << 4) | ((currentValue & 0xF0) >> 4);

		registers.SetFlags(newValue == 0, 0, 0, 0);
		break;
	}
	case IN_SRL:
	{
		const u8 shiftedBit = (currentValue & 0x1);
		newValue = (currentValue >> 1);

		registers.SetFlags(newValue == 0, 0, 0, shiftedBit);
		break;
	}
	case IN_RES:
	{
		newValue = currentValue & ~(1 << bitIndex);
		break;
	}
	case IN_SET:
	{
		newValue = currentValue | (1 << bitIndex);
		break;
	}
	case IN_BIT:
	{
		const u8 bitValue = currentValue & (1 << bitIndex);
		registers.SetFlags(bitValue == 0, 0, 1, -1);
		return;
	}
	default:
	{
		NO_IMPL("Invalid CB opcode!");
		break;
	}
	}

	if (reg == RT_HL)
	{
		EMU::GetBUS()->WriteByte(regValue, newValue);
		EMU::Cycle(1);
	}
	else
	{
		registers.Write(reg, newValue);
	}
}

void CPU::Instruction_AND_OR_XOR(const Instruction* instruction)
{
	EMU::Cycle(1);

	const u8 reg_a_value = registers.Read(RT_A);

	u8 result = reg_a_value;
	switch (instruction->type)
	{
	case IN_AND:
	{
		result &= (fetched_data & 0xFF);
		break;
	}
	case IN_OR:
	{
		result |= (fetched_data & 0xFF);
		break;
	}
	case IN_XOR:
	{
		result ^= (fetched_data & 0xFF);
		break;
	}
	default:
	{
		NO_IMPL("Instruction_AND_OR_XOR broken!");
		break;
	}
	}

	registers.Write(RT_A, result);
	registers.SetFlags(result == 0, 0, instruction->type == IN_AND, 0);
}

void CPU::Instruction_CP(const Instruction* instruction)
{
	EMU::Cycle(1);

	const u8 reg_a_value = registers.Read(RT_A);

	const i16 result = (i16)reg_a_value - (i16)fetched_data;
	const i16 hResult = (i16)(reg_a_value & 0x0F) - (i16)(fetched_data & 0x0F);

	const u8 hFlag = hResult < 0;
	const u8 cFlag = result < 0;

	registers.SetFlags(result == 0, 1, hFlag, cFlag);
}

void CPU::Instruction_PUSH_POP(const Instruction* instruction)
{
	EMU::Cycle(1);

	if (instruction->type == IN_PUSH)
	{
		Stack_PushWord(registers.Read(instruction->reg_1));
		EMU::Cycle(3);
	}
	else
	{
		const u16 poppedValue = Stack_PopWord();
		EMU::Cycle(2);

		if (instruction->reg_1 == RT_AF)
		{
			registers.Write(instruction->reg_1, poppedValue & 0xFFF0);
		}
		else
		{
			registers.Write(instruction->reg_1, poppedValue);
		}
	}
}

void CPU::Instruction_STOP(const Instruction* instruction)
{
	// TODO
}

void CPU::Instruction_HALT(const Instruction* instruction)
{
	halted = true;
	
	// TODO handle interrupts
}

void CPU::Instruction_RLA_RRA(const Instruction* instruction)
{
	EMU::Cycle(1);

	const u8 reg_a = registers.Read(RT_A);
	const u8 cFlag = registers.GetCarryFlag();

	u8 newReg_a;
	u8 newCFlag;
	if (instruction->type == IN_RLA)
	{
		newCFlag = (reg_a & 0x80) > 0;
		newReg_a = reg_a << 1 | cFlag;
	}
	else
	{
		newCFlag = reg_a & 0x1;
		newReg_a = (reg_a >> 1) | (cFlag << 7);
	}

	registers.Write(RT_A, newReg_a);
	registers.SetFlags(0, 0, 0, newCFlag);
}

void CPU::Instruction_CPL(const Instruction* instruction)
{
	EMU::Cycle(1);

	const u8 reg_a = registers.Read(RT_A);
	registers.Write(RT_A, ~reg_a);
	registers.SetFlags(-1, 1, 1, -1);
}

void CPU::Instruction_DAA(const Instruction* instruction)
{
	EMU::Cycle(1);

	const u8 reg_a = registers.Read(RT_A);
	const u8 subFlag = registers.GetSubtractionFlag();
	const u8 hFlag = registers.GetHalfCarryFlag();
	u8 cFlag = 0;


	u8 correction = 0;
	if (hFlag || (!subFlag && (reg_a & 0xF) > 9))
	{
		correction = 0x6;
	}

	if (registers.GetCarryFlag() || (!subFlag && reg_a > 0x99))
	{
		correction |= 0x60;
		cFlag = 1;
	}

	const u8 result = (reg_a + (subFlag ? -correction : correction)) & 0xFF;

	registers.Write(RT_A, result);
	registers.SetFlags(result == 0, -1, 0, cFlag);
}

void CPU::Instruction_SCF_CCF(const Instruction* instruction)
{
	EMU::Cycle(1);

	const u8 cFlag = instruction->type == IN_SCF ? 1 : registers.GetCarryFlag() == 0;

	registers.SetFlags(-1, 0, 0, cFlag);
}
