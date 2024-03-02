#include "cpu_registers.h"

using namespace GB;

u16 GB::CPU_Registers::Read(RegisterType type)
{
	switch (type)
	{
	case RegisterType::A: return A;
	case RegisterType::F: return F;
	case RegisterType::B: return B;
	case RegisterType::C: return C;
	case RegisterType::D: return D;
	case RegisterType::E: return E;
	case RegisterType::H: return H;
	case RegisterType::L: return L;
	case RegisterType::AF: return A << 8 | F;
	case RegisterType::BC: return B << 8 | C;
	case RegisterType::DE: return D << 8 | E;
	case RegisterType::HL: return H << 8 | L;
	case RegisterType::SP: return SP;
	case RegisterType::PC: return PC;
	default:
		return 0;
	}
}

void CPU_Registers::Write(RegisterType type, u16 newValue)
{
	switch (type)
	{
	case RegisterType::A: A = newValue & 0xFF; break;
	case RegisterType::F: F = newValue & 0xFF; break;
	case RegisterType::B: B = newValue & 0xFF; break;
	case RegisterType::C: C = newValue & 0xFF; break;
	case RegisterType::D: D = newValue & 0xFF; break;
	case RegisterType::E: E = newValue & 0xFF; break;
	case RegisterType::H: H = newValue & 0xFF; break;
	case RegisterType::L: L = newValue & 0xFF; break;
	case RegisterType::AF:
	{
		A = (newValue & 0xFF00) >> 8;
		F = newValue & 0x00FF;
		break;
	}
	case RegisterType::BC:
	{
		B = (newValue & 0xFF00) >> 8;
		C = newValue & 0x00FF;
		break;
	}
	case RegisterType::DE:
	{
		D = (newValue & 0xFF00) >> 8;
		E = newValue & 0x00FF;
		break;
	}
	case RegisterType::HL:
	{
		H = (newValue & 0xFF00) >> 8;
		L = newValue & 0x00FF;
		break;
	}
	case RegisterType::SP: SP = newValue; break;
	case RegisterType::PC: PC = newValue; break;
	}
}

void CPU_Registers::Increment(RegisterType type)
{
	const u16 current = Read(type);
	const u16 newValue = current + 1;
	Write(type, newValue);
}

void CPU_Registers::Decrement(RegisterType type)
{
	const u16 current = Read(type);
	const u16 newValue = current - 1;
	Write(type, newValue);
}

void CPU_Registers::IncrementPC()
{
	PC++;
}

void CPU_Registers::SetPC(u16 newValue)
{
	PC = newValue;
}

u16 CPU_Registers::GetPC() const
{
	return PC;
}

bool CPU_Registers::IsWordSize(RegisterType type)
{
	return type >= RegisterType::AF;
}

void CPU_Registers::SetFlags(i8 zeroFlag, i8 subtractionFlag, i8 halfCarryFlag, i8 carryFlag)
{
	SetFlag(zeroFlag, 7);
	SetFlag(subtractionFlag, 6);
	SetFlag(halfCarryFlag, 5);
	SetFlag(carryFlag, 4);
}

void CPU_Registers::SetFlag(u8 value, u8 bitOffset)
{
	if (value != -1)
	{
		if (value)
		{
			F |= (1 << bitOffset);
		}
		else
		{
			F &= ~(1 << bitOffset);
		}
	}
}

u8 CPU_Registers::GetCarryFlag() const
{
	return (F & (1 << 4)) > 0;
}

u8 CPU_Registers::GetHalfCarryFlag() const
{
	return (F & (1 << 5)) > 0;
}

u8 CPU_Registers::GetSubtractionFlag() const
{
	return (F & (1 << 6)) > 0;
}

u8 CPU_Registers::GetZeroFlag() const
{
	return (F & (1 << 7)) > 0;
}

std::string CPU_Registers::GetFlagsString() const
{
	std::string flagsString;

	flagsString += GetZeroFlag() ? 'Z' : '-';
	flagsString += GetSubtractionFlag() ? 'N' : '-';
	flagsString += GetHalfCarryFlag() ? 'H' : '-';
	flagsString += GetCarryFlag() ? 'C' : '-';

	return flagsString;
}
