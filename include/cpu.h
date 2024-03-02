#pragma once

#include <common.h>
#include "Instructions.h"
#include <functional>
#include <vector>
#include "cpu_registers.h"

namespace GB
{
	class CPU;

	class CPU
	{

	public:

		CPU();

		bool Step();

		void Sleep(u32 ms);

	private:

		void FetchInstruction();
		void FetchData();
		void Execute();

	private:

		void Instruction_NOP();
		void Instruction_LD();
		void Instruction_LDH();

		void Instruction_INC();
		void Instruction_DEC();

		void Instruction_ADD();
		void Instruction_ADC();

		void Instruction_SUB();
		void Instruction_SBC();

		void Instruction_RLCA();
		void Instruction_RRCA();

		void Instruction_JP_JR();
		void Instruction_CALL_RST();

		void Instruction_RET_RETI();

		void Instruction_EI_DI();

		void Instruction_CB();

		void Instruction_AND_OR_XOR();

		void Instruction_CP();

		void Instruction_PUSH_POP();

		void Instruction_STOP();

		void Instruction_HALT();

		void Instruction_RLA_RRA();

		void Instruction_CPL();

		void Instruction_DAA();

		void Instruction_SCF_CCF();

	private:

		u8 ReadBytePC();
		u16 ReadWordPC();

		void SetMemDest(u16 address);

		void Stack_PushByte(u8 value);
		void Stack_PushWord(u16 value);
		u8 Stack_PopByte();
		u16 Stack_PopWord();

		bool IsConditionMet(const CondType condition) const;

	public:

		u8 GetInterruptFlags() const;
		void SetInterruptFlags(u8 newFlags);
		bool IsInterruptSet(const IntType type) const;

		u8 GetInterruptEnabledFlags() const;
		void SetInterruptEnabledFlags(u8 newFlags);
		bool IsInterruptEnabled(const IntType type) const;

		void HandleInterrupts();
		void RequestInterrupt(const IntType type);

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

		CPU_Registers registers;

		u16 stackCounter = 0;

		std::vector<u16> stackDebug;

	private:

		bool enableInterrupts = false;
	};

}