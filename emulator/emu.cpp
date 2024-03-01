#include <emu.h>
#include <cart.h>
#include <cpu.h>
#include "bus.h"
#include "window.h"

#include <cstdio>
#include <chrono>
#include "timer.h"
#include "lcd.h"
#include "ram.h"
#include "io.h"
#include "ppu.h"

using namespace GB;

/*
  Emu components:

  |Cart|
  |CPU|
  |Address Bus|
  |PPU|
  |Timer|

*/

EMU::EMU()
{
    cpu = std::make_unique<CPU>();
	bus = std::make_unique<MEM_BUS>();
	io = std::make_unique<IO>();
	timer = std::make_unique<Timer>();
	lcd = std::make_unique<LCD>();
	ram = std::make_unique<RAM>();
	window = std::make_unique<Window>();
	cartridge = std::make_unique<Cartridge>();
	ppu = std::make_unique<PPU>();
}

int EMU::Run(int argc, char** argv)
{
	if (argc < 3)
	{
		printf("Usage: <rom_folder> <rom_file>\n");
		return -1;
	}

	std::string romPath = argv[1];
	romPath += argv[2];

	if (!cartridge->Load(romPath))
	{
		printf("Failed to load ROM file: %s\n", argv[1]);
		return -2;
	}

	printf("Cart loaded..\n");

	bIsRunning = true;

	std::thread cpuThread(&EMU::ExecuteCPU, this);

	u32 previousFrame = 0;

	while (bIsRunning)
	{
		window->Delay(1000);
		window->HandleEvents();

		if (previousFrame != EMU::GetPPU()->GetCurrentFrame())
		{
			window->UpdateWindow();
		}

		previousFrame = EMU::GetPPU()->GetCurrentFrame();
	}
	cpuThread.join();

	return 0;
}

void EMU::Delay(u32 MS)
{
	window->Delay(MS);
}

EMU* EMU::GetEMU()
{
	static std::unique_ptr<EMU> emu(new EMU);

	return emu.get();
}

void EMU::Cycle(u8 amount)
{
	if (amount == 0)
	{
		return;
	}

	for (int i = 0; i < amount; i++)
	{
		for (int n = 0; n < 4; n++)
		{
			GetEMU()->cycles++;
			GetEMU()->timer->Tick(1);
			GetEMU()->ppu->Tick();
		}

		GetBUS()->DMA_Tick();
	}
}

u32 EMU::GetCycles() const
{
	return cycles;
}

void EMU::Shutdown()
{
	bIsRunning = false;
}

void* EMU::ExecuteCPU()
{
	while (bIsRunning)
	{
		if (bPaused)
		{
			Delay(10);
			continue;
		}

		if (!cpu->Step())
		{
			printf("CPU Stopped\n");
			return 0;
		}

		Ticks++;

		//cpu->Sleep(1);
	}

	return nullptr;
}

void EMU::DebugUpdate()
{
	if (bus->ReadByte(0xFF02) == 0x81)
	{
		if (DebugBufferMsgSize == 0 && DebugBuffer[0] != 0)
		{
			DebugBuffer = {};
		}
		char newChar = bus->ReadByte(0xFF01);
		DebugBuffer[DebugBufferMsgSize++] = newChar;
		bus->WriteByte(0xFF02, 0);
		if (newChar == '\n' &&
			DebugBufferMsgSize >= 2 &&
			(DebugBuffer[DebugBufferMsgSize - 2] == '\n' || DebugBuffer[0] == '\n'))
		{
			msgUpdated = true;
			DebugBufferMsgSize = 0;
		}
	}
}

void EMU::DebugPrint()
{
	if (msgUpdated)
	{
		printf("DBG: %s", DebugBuffer.data());
		msgUpdated = false;
	}
}

CPU* EMU::GetCPU()
{
	return GetEMU()->cpu.get();
}

MEM_BUS* EMU::GetBUS()
{
	return GetEMU()->bus.get();
}

IO* EMU::GetIO()
{
	return GetEMU()->io.get();
}

Timer* EMU::GetTimer()
{
	return GetEMU()->timer.get();
}

RAM* EMU::GetRAM()
{
	return GetEMU()->ram.get();
}

Window* EMU::GetWindow()
{
	return GetEMU()->window.get();
}

LCD* EMU::GetLCD()
{
	return GetEMU()->lcd.get();
}

Cartridge* EMU::GetCartridge()
{
	return GetEMU()->cartridge.get();
}

PPU* EMU::GetPPU()
{
	return GetEMU()->ppu.get();
}
