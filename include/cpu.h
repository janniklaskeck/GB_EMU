#pragma once

#include <common.h>
#include "Instructions.h"
#include <functional>
#include <vector>

namespace GB
{
	enum InterruptType
	{
		IT_VBlank = 1,
		IT_LCD_Stat = 2,
		IT_Timer = 4,
		IT_Serial = 8,
		IT_Joypad = 16
	};

	class CPU;
	using Instr_Function = std::function<void(CPU&, const Instruction*)>;

	class CPU
	{
		struct Registers
		{
			u8 A, F;
			u8 B, C;
			u8 D, E;
			u8 H, L;

			u16 SP;
			u16 PC;

			u16 Read(reg_type type)
			{
				switch (type)
				{
				case RT_A: return A;
				case RT_F: return F;
				case RT_B: return B;
				case RT_C: return C;
				case RT_D: return D;
				case RT_E: return E;
				case RT_H: return H;
				case RT_L: return L;
				case RT_AF: return A << 8 | F;
				case RT_BC: return B << 8 | C;
				case RT_DE: return D << 8 | E;
				case RT_HL: return H << 8 | L;
				case RT_SP: return SP;
				case RT_PC: return PC;
				default:
					return 0;
				}
			}

			void Write(reg_type type, u16 newValue)
			{
				switch (type)
				{
				case RT_A: A = newValue & 0xFF; break;
				case RT_F: F = newValue & 0xFF; break;
				case RT_B: B = newValue & 0xFF; break;
				case RT_C: C = newValue & 0xFF; break;
				case RT_D: D = newValue & 0xFF; break;
				case RT_E: E = newValue & 0xFF; break;
				case RT_H: H = newValue & 0xFF; break;
				case RT_L: L = newValue & 0xFF; break;
				case RT_AF:
				{
					A = (newValue & 0xFF00) >> 8;
					F = newValue & 0x00FF;
					break;
				}
				case RT_BC:
				{
					B = (newValue & 0xFF00) >> 8;
					C = newValue & 0x00FF;
					break;
				}
				case RT_DE: {
					D = (newValue & 0xFF00) >> 8;
					E = newValue & 0x00FF;
					break;
				}
				case RT_HL:
				{
					H = (newValue & 0xFF00) >> 8;
					L = newValue & 0x00FF;
					break;
				}
				case RT_SP: SP = newValue; break;
				case RT_PC: PC = newValue; break;
				}
			}

			void Increment(reg_type type)
			{
				const u16 current = Read(type);
				const u16 newValue = current + 1;
				Write(type, newValue);
			}

			void Decrement(reg_type type)
			{
				const u16 current = Read(type);
				const u16 newValue = current - 1;
				Write(type, newValue);
			}

			static bool IsWordSize(reg_type type)
			{
				return type >= RT_AF;
			}

			u8 GetZeroFlag() const
			{
				return (F & (1 << 7)) > 0;
			}

			u8 GetSubtractionFlag() const
			{
				return (F & (1 << 6)) > 0;
			}

			u8 GetHalfCarryFlag() const
			{
				return (F & (1 << 5)) > 0;
			}

			u8 GetCarryFlag() const
			{
				return (F & (1 << 4)) > 0;
			}

			void SetFlags(i8 zeroFlag, i8 subtractionFlag, i8 halfCarryFlag, i8 carryFlag)
			{
				if (zeroFlag != -1)
				{
					if (zeroFlag)
					{
						F |= 1 << 7;
					}
					else
					{
						F &= ~(1 << 7);
					}
				}

				if (subtractionFlag != -1)
				{
					if (subtractionFlag)
					{
						F |= 1 << 6;
					}
					else
					{
						F &= ~(1 << 6);
					}
				}

				if (halfCarryFlag != -1)
				{
					if (halfCarryFlag)
					{
						F |= 1 << 5;
					}
					else
					{
						F &= ~(1 << 5);
					}
				}

				if (carryFlag != -1)
				{
					if (carryFlag)
					{
						F |= 1 << 4;
					}
					else
					{
						F &= ~(1 << 4);
					}
				}
			}

			std::string GetFlagsString() const
			{
				std::string flagsString;

				flagsString += GetZeroFlag() ? 'Z' : '-';
				flagsString += GetSubtractionFlag() ? 'N' : '-';
				flagsString += GetHalfCarryFlag() ? 'H' : '-';
				flagsString += GetCarryFlag() ? 'C' : '-';

				return flagsString;
			}
		};

	public:

		CPU();

		bool Step();

		void Sleep(u32 ms);

	private:

		void FetchInstruction();
		void FetchData();
		void Execute();

	private:

		Instr_Function GetFunction(const Instruction* instruction) const;

		void Instruction_NOP(const Instruction* instruction);
		void Instruction_LD(const Instruction* instruction);
		void Instruction_LDH(const Instruction* instruction);

		void Instruction_INC(const Instruction* instruction);
		void Instruction_DEC(const Instruction* instruction);

		void Instruction_ADD(const Instruction* instruction);
		void Instruction_ADC(const Instruction* instruction);

		void Instruction_SUB(const Instruction* instruction);
		void Instruction_SBC(const Instruction* instruction);

		void Instruction_RLCA(const Instruction* instruction);
		void Instruction_RRCA(const Instruction* instruction);

		void Instruction_JP_JR(const Instruction* instruction);
		void Instruction_CALL_RST(const Instruction* instruction);

		void Instruction_RET_RETI(const Instruction* instruction);

		void Instruction_EI_DI(const Instruction* instruction);

		void Instruction_CB(const Instruction* instruction);

		void Instruction_AND_OR_XOR(const Instruction* instruction);

		void Instruction_CP(const Instruction* instruction);

		void Instruction_PUSH_POP(const Instruction* instruction);

		void Instruction_STOP(const Instruction* instruction);

		void Instruction_HALT(const Instruction* instruction);

		void Instruction_RLA_RRA(const Instruction* instruction);

		void Instruction_CPL(const Instruction* instruction);

		void Instruction_DAA(const Instruction* instruction);

		void Instruction_SCF_CCF(const Instruction* instruction);

	private:

		u8 ReadByteBUS(u16 address);
		u8 ReadBytePC();
		u16 ReadWordPC();

		void SetMemDest(u16 address);

		void Stack_PushByte(u8 value);
		void Stack_PushWord(u16 value);
		u8 Stack_PopByte();
		u16 Stack_PopWord();

		bool IsConditionMet(const cond_type condition) const;

	public:

		u8 GetInterruptFlags() const;
		void SetInterruptFlags(u8 newFlags);
		bool IsInterruptSet(const InterruptType type) const;

		u8 GetInterruptEnabledFlags() const;
		void SetInterruptEnabledFlags(u8 newFlags);
		bool IsInterruptEnabled(const InterruptType type) const;

		void HandleInterrupts();
		void RequestInterrupt(const InterruptType type);

	private:

		std::string GetInstructionDebugString() const;

	private:

		u16 fetched_data = 0;
		u16 mem_dest = 0;
		bool mem_dest_isMem = false;
		u8 opcode = 0;
		Instruction* instruction = nullptr;

		bool halted = false;
		bool stepping = false;
		bool interruptsEnabled = false;

		u8 IF_Flags = 0;
		u8 IE_Flags = 0;

		Registers registers;

		u16 stackCounter = 0;

		std::vector<u16> stackDebug;

	private:

		bool enableInterrupts = false;
	};

}