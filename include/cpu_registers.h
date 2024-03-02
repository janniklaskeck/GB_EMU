#pragma once

#include "common.h"
#include "Instructions.h"

namespace GB
{
	struct CPU_Registers
	{
		u8 A, F;
		u8 B, C;
		u8 D, E;
		u8 H, L;

		u16 SP;
		u16 PC;

	public:

		u16 Read(RegisterType type);

		void Write(RegisterType type, u16 newValue);

		void Increment(RegisterType type);

		void Decrement(RegisterType type);

	public:

		u16 GetPC() const;

		void SetPC(u16 newValue);

		void IncrementPC();

	public:

		static bool IsWordSize(RegisterType type);

	public:

		u8 GetZeroFlag() const;

		u8 GetSubtractionFlag() const;

		u8 GetHalfCarryFlag() const;

		u8 GetCarryFlag() const;

		void SetFlags(i8 zeroFlag, i8 subtractionFlag, i8 halfCarryFlag, i8 carryFlag);

		std::string GetFlagsString() const;

	private:

		void SetFlag(u8 value, u8 bitOffset);
	};
}