#pragma once

#include "common.h"

#include <memory>

namespace GB
{
	class MEM_BUS
	{
	public:

		MEM_BUS() = default;

	public:

		u8 ReadByte(u16 address) const;

		void WriteByte(u16 address, u8 value);

		void WriteWord(u16 address, u16 value);

		void DMA_Start(u8 start);

		void DMA_Tick();

		bool DMA_TransferActive() const;

	private:

		bool dmaTransferActive = false;
		u8 dmaCurrentByte = 0;
		u8 dmaValue = 0;
		u8 dmaStartDelay = 0;
	};
}
