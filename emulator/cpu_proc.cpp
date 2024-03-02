#include <cpu.h>
#include "bus.h"
#include "Instructions.h"
#include "emu.h"

using namespace GB;

void CPU::Stack_PushByte(u8 value)
{
	registers.Decrement(RegisterType::SP);
	const u16 SP_Value = registers.Read(RegisterType::SP);
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
	const u16 SP_Value = registers.Read(RegisterType::SP);
	const u8 value = EMU::GetBUS()->ReadByte(SP_Value);
	registers.Increment(RegisterType::SP);

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

bool CPU::IsConditionMet(const CondType condition) const
{

	switch (condition)
	{
	case CondType::NZ: return !registers.GetZeroFlag();
	case CondType::Z: return registers.GetZeroFlag();
	case CondType::NC: return !registers.GetCarryFlag();
	case CondType::C: return registers.GetCarryFlag();
	case CondType::NONE: return true;
	default:
	{
		exit(-7);
		break;
	}
	}
}

void CPU::Instruction_NOP()
{
	EMU::Cycle(1);
}

void CPU::Instruction_LD()
{
	EMU::Cycle(1);

	if (instruction->mode == AddrMode::R_HLI || instruction->mode == AddrMode::HLI_R)
	{
		registers.Increment(RegisterType::HL);
	}
	else if (instruction->mode == AddrMode::R_HLD || instruction->mode == AddrMode::HLD_R)
	{
		registers.Decrement(RegisterType::HL);
	}

	if (mem_dest_isMem)
	{
		EMU::Cycle(1);

		if (CPU_Registers::IsWordSize(instruction->reg_2))
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

	if (instruction->mode == AddrMode::HL_SPD)
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

void CPU::Instruction_LDH()
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

void CPU::Instruction_INC()
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

	if (isWordReg && instruction->mode != AddrMode::MR)
	{
		return;
	}

	const u8 zFlag = (newValue & 0xFF) == 0;
	const u8 hFlag = ((fetched_data & 0xF) + 1) > 0xF;
	registers.SetFlags(zFlag, 0, hFlag, -1);
}

void CPU::Instruction_DEC()
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

	if (isWordReg && instruction->mode != AddrMode::MR)
	{
		return;
	}

	const u8 zFlag = (newValue & 0xFF) == 0;
	const u8 hFlag = (((i16)fetched_data & 0xF) - 1) < 0;
	registers.SetFlags(zFlag, 1, hFlag, -1);
}

void CPU::Instruction_ADD()
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

		if (instruction->reg_1 == RegisterType::SP)
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

void CPU::Instruction_ADC()
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

void CPU::Instruction_SUB()
{
	EMU::Cycle(1);

	const u16 currentValue = registers.Read(instruction->reg_1);
	u32 newValue = currentValue - fetched_data;

	if (instruction->type == InstrType::SBC)
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

void CPU::Instruction_SBC()
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

void CPU::Instruction_RLCA()
{
	EMU::Cycle(1);

	const u8 currentValue = registers.Read(RegisterType::A);
	const u8 bitZero = (currentValue & 0x80) > 0;
	const u8 newValue = (currentValue << 1) | bitZero;

	registers.Write(RegisterType::A, newValue);
	registers.SetFlags(0, 0, 0, bitZero);
}

void CPU::Instruction_RRCA()
{
	EMU::Cycle(1);

	const u8 currentValue = registers.Read(RegisterType::A);
	const u8 bitZero = currentValue & 0x1;
	u8 newValue = currentValue >> 1;
	newValue |= bitZero << 7;

	registers.Write(RegisterType::A, newValue);
	registers.SetFlags(0, 0, 0, bitZero);
}

void CPU::Instruction_JP_JR()
{
	EMU::Cycle(1);

	if (instruction->mode == AddrMode::R)
	{
		const u16 jumpAddress = registers.Read(instruction->reg_1);
		registers.SetPC(jumpAddress);
		return;
	}

	if (instruction->mode == AddrMode::D8)
	{
		const u16 reg_PC = registers.GetPC();
		const i8 signedOffset = fetched_data;
		fetched_data = reg_PC + signedOffset;
	}

	const u16 jumpAddress = fetched_data;

	if (IsConditionMet(instruction->cond))
	{
		registers.SetPC(jumpAddress);

		EMU::Cycle(1);
	}
}

void CPU::Instruction_CALL_RST()
{
	EMU::Cycle(1);

	if (instruction->type == InstrType::RST)
	{
		fetched_data = instruction->param;
	}

	const u16 jumpAddress = fetched_data;

	if (IsConditionMet(instruction->cond))
	{
		const u16 nextInstruction = registers.GetPC();
		Stack_PushWord(nextInstruction);
		registers.SetPC(jumpAddress);

		EMU::Cycle(3);
	}
}

void CPU::Instruction_RET_RETI()
{
	EMU::Cycle(2);

	if (instruction->type == InstrType::RETI)
	{
		enableInterrupts = true;
	}

	if (IsConditionMet(instruction->cond))
	{
		const u16 jumpAddress = Stack_PopWord();
		EMU::Cycle(2);

		registers.SetPC(jumpAddress);

		if (instruction->cond != CondType::NONE)
		{
			EMU::Cycle(1);
		}
	}
}

void CPU::Instruction_EI_DI()
{
	EMU::Cycle(1);

	if (instruction->type == InstrType::EI)
	{
		enableInterrupts = true;
		return;
	}

	enableInterrupts = false;
	interruptsEnabled = false;
}

void CPU::Instruction_CB()
{
	EMU::Cycle(2);

	const u8 CB_opcode = (fetched_data >> 3);
	const u8 bitIndex = (fetched_data >> 3) & 0b111;

	RegisterType rt_lookup[] = {
	RegisterType::B,
	RegisterType::C,
	RegisterType::D,
	RegisterType::E,
	RegisterType::H,
	RegisterType::L,
	RegisterType::HL,
	RegisterType::A
	};

	const RegisterType reg = rt_lookup[fetched_data & 0b111];
	InstrType CB_Type;
	if (CB_opcode < 8)
	{
		CB_Type = (InstrType)((u16)InstrType::RLC + CB_opcode);
	}
	else
	{
		CB_Type = (InstrType)((u16)InstrType::BIT + (CB_opcode >> 3) - 1);
	}

	const u16 regValue = registers.Read(reg);
	const u8 currentValue = reg == RegisterType::HL ? EMU::GetBUS()->ReadByte(regValue) : regValue;
	u8 newValue = 0;
	switch (CB_Type)
	{
	case InstrType::RLC:
	{
		const u8 shiftedBit = (currentValue & 0x80) > 0;
		newValue = (currentValue << 1) | shiftedBit;

		registers.SetFlags(newValue == 0, 0, 0, shiftedBit);
		break;
	}
	case InstrType::RRC:
	{
		const u8 shiftedBit = (currentValue & 0x1);
		newValue = (currentValue >> 1) | (shiftedBit << 7);

		registers.SetFlags(newValue == 0, 0, 0, shiftedBit);
		break;
	}
	case InstrType::RL:
	{
		const u8 carryFlag = registers.GetCarryFlag();
		const u8 shiftedBit = (currentValue & 0x80) > 0;
		newValue = (currentValue << 1) | carryFlag;

		registers.SetFlags(newValue == 0, 0, 0, shiftedBit);
		break;
	}
	case InstrType::RR:
	{
		const u8 carryFlag = registers.GetCarryFlag();
		const u8 shiftedBit = (currentValue & 0x1);
		newValue = (currentValue >> 1) | (carryFlag << 7);

		registers.SetFlags(newValue == 0, 0, 0, shiftedBit);
		break;
	}
	case InstrType::SLA:
	{
		const u8 shiftedBit = (currentValue & 0x80) > 0;
		newValue = (currentValue << 1);

		registers.SetFlags(newValue == 0, 0, 0, shiftedBit);
		break;
	}
	case InstrType::SRA:
	{
		const u8 MSB = (currentValue & 0x80);
		const u8 shiftedBit = (currentValue & 0x1);
		newValue = (currentValue >> 1) | MSB;

		registers.SetFlags(newValue == 0, 0, 0, shiftedBit);
		break;
	}
	case InstrType::SWAP:
	{
		newValue = ((currentValue & 0x0F) << 4) | ((currentValue & 0xF0) >> 4);

		registers.SetFlags(newValue == 0, 0, 0, 0);
		break;
	}
	case InstrType::SRL:
	{
		const u8 shiftedBit = (currentValue & 0x1);
		newValue = (currentValue >> 1);

		registers.SetFlags(newValue == 0, 0, 0, shiftedBit);
		break;
	}
	case InstrType::RES:
	{
		newValue = currentValue & ~(1 << bitIndex);
		break;
	}
	case InstrType::SET:
	{
		newValue = currentValue | (1 << bitIndex);
		break;
	}
	case InstrType::BIT:
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

	if (reg == RegisterType::HL)
	{
		EMU::GetBUS()->WriteByte(regValue, newValue);
		EMU::Cycle(1);
	}
	else
	{
		registers.Write(reg, newValue);
	}
}

void CPU::Instruction_AND_OR_XOR()
{
	EMU::Cycle(1);

	const u8 reg_a_value = registers.Read(RegisterType::A);

	u8 result = reg_a_value;
	switch (instruction->type)
	{
	case InstrType::AND:
	{
		result &= (fetched_data & 0xFF);
		break;
	}
	case InstrType::OR:
	{
		result |= (fetched_data & 0xFF);
		break;
	}
	case InstrType::XOR:
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

	registers.Write(RegisterType::A, result);
	registers.SetFlags(result == 0, 0, instruction->type == InstrType::AND, 0);
}

void CPU::Instruction_CP()
{
	EMU::Cycle(1);

	const u8 reg_a_value = registers.Read(RegisterType::A);

	const i16 result = (i16)reg_a_value - (i16)fetched_data;
	const i16 hResult = (i16)(reg_a_value & 0x0F) - (i16)(fetched_data & 0x0F);

	const u8 hFlag = hResult < 0;
	const u8 cFlag = result < 0;

	registers.SetFlags(result == 0, 1, hFlag, cFlag);
}

void CPU::Instruction_PUSH_POP()
{
	EMU::Cycle(1);

	if (instruction->type == InstrType::PUSH)
	{
		Stack_PushWord(registers.Read(instruction->reg_1));
		EMU::Cycle(3);
	}
	else
	{
		const u16 poppedValue = Stack_PopWord();
		EMU::Cycle(2);

		if (instruction->reg_1 == RegisterType::AF)
		{
			registers.Write(instruction->reg_1, poppedValue & 0xFFF0);
		}
		else
		{
			registers.Write(instruction->reg_1, poppedValue);
		}
	}
}

void CPU::Instruction_STOP()
{
	// TODO
}

void CPU::Instruction_HALT()
{
	halted = true;

	// TODO handle interrupts
}

void CPU::Instruction_RLA_RRA()
{
	EMU::Cycle(1);

	const u8 reg_a = registers.Read(RegisterType::A);
	const u8 cFlag = registers.GetCarryFlag();

	u8 newReg_a;
	u8 newCFlag;
	if (instruction->type == InstrType::RLA)
	{
		newCFlag = (reg_a & 0x80) > 0;
		newReg_a = reg_a << 1 | cFlag;
	}
	else
	{
		newCFlag = reg_a & 0x1;
		newReg_a = (reg_a >> 1) | (cFlag << 7);
	}

	registers.Write(RegisterType::A, newReg_a);
	registers.SetFlags(0, 0, 0, newCFlag);
}

void CPU::Instruction_CPL()
{
	EMU::Cycle(1);

	const u8 reg_a = registers.Read(RegisterType::A);
	registers.Write(RegisterType::A, ~reg_a);
	registers.SetFlags(-1, 1, 1, -1);
}

void CPU::Instruction_DAA()
{
	EMU::Cycle(1);

	const u8 reg_a = registers.Read(RegisterType::A);
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

	registers.Write(RegisterType::A, result);
	registers.SetFlags(result == 0, -1, 0, cFlag);
}

void CPU::Instruction_SCF_CCF()
{
	EMU::Cycle(1);

	const u8 cFlag = instruction->type == InstrType::SCF ? 1 : registers.GetCarryFlag() == 0;

	registers.SetFlags(-1, 0, 0, cFlag);
}
