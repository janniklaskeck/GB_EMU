#pragma once

#include "common.h"

#include <memory>
#include <array>
#include <thread>

namespace GB
{
    class Window;
    class CPU;
    class Cartridge;
    class IO;
    class RAM;
    class Timer;
    class MEM_BUS;
    class LCD;
    class PPU;

    class EMU
    {
        EMU();
    public:

        static EMU* GetEMU();

    public:

        int Run(int argc, char** argv);

        void Shutdown();

        static void Cycle(u8 amount);

        u32 GetCycles() const;

        static CPU*     GetCPU();
        static MEM_BUS* GetBUS();
        static IO*      GetIO();
        static Timer*   GetTimer();
        static RAM*     GetRAM();
        static Window*  GetWindow();
        static LCD*     GetLCD();
        static Cartridge* GetCartridge();
        static PPU*     GetPPU();

    private:

        void* ExecuteCPU();

        void Delay(u32 MS);

    public:

		void DebugUpdate();

		void DebugPrint();

    private:

        u32 cycles = 0;

        std::unique_ptr<CPU> cpu;
        std::unique_ptr<MEM_BUS> bus;
        std::unique_ptr<IO> io;
        std::unique_ptr<Timer> timer;
        std::unique_ptr<RAM> ram;
        std::unique_ptr<Window> window;
        std::unique_ptr<LCD> lcd;
        std::unique_ptr<Cartridge> cartridge;
        std::unique_ptr<PPU> ppu;

        std::array<char, 1024> DebugBuffer{};
        u32 DebugBufferMsgSize = 0;
        bool msgUpdated = false;

    private:

		bool bPaused = false;
		bool bIsRunning = false;
		u64 Ticks = 0;
    };

}
