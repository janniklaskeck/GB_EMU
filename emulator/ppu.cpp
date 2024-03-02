#include <ppu.h>
#include "emu.h"
#include "lcd.h"
#include "cpu.h"
#include "window.h"

using namespace GB;

void PPU::Tick()
{
	line_ticks++;

	LCD* lcd = EMU::GetLCD();

	switch (lcd->Get_PPU_Mode())
	{
	case LCD_Mode::HBLANK:
	{
		TICK_HBLANK();
		break;
	}
	case LCD_Mode::VBLANK:
	{
		Tick_VLBANK();
		break;
	}
	case LCD_Mode::OAM:
	{
		Tick_OAM();
		break;
	}
	case LCD_Mode::XFER:
	{
		Tick_XFER();
		break;
	}
	default:
		break;

	}
}

u8 PPU::ReadOAM_Byte(u16 address) const
{
	if (address >= TOTAL_OAM_SIZE)
	{
		address -= OAM_START;
	}

	return oam[address];
}

void PPU::WriteOAM_Byte(u16 address, u8 value)
{
	if (address >= TOTAL_OAM_SIZE)
	{
		address -= OAM_START;
	}

	oam[address] = value;
}

bool PPU::IsOAM_Addr(u16 address)
{
	return address >= OAM_START && address <= OAM_END;
}

void PPU::Tick_OAM()
{
	if (line_ticks >= 80)
	{
		EMU::GetLCD()->Set_PPU_Mode(LCD_Mode::XFER);
	}
}

void PPU::Tick_XFER()
{
	if (line_ticks >= (80 + 172))
	{
		EMU::GetLCD()->Set_PPU_Mode(LCD_Mode::HBLANK);
	}
}

void PPU::Tick_VLBANK()
{
	if (line_ticks >= TICKS_PER_LINE)
	{
		EMU::GetLCD()->IncrementLY();

		if (EMU::GetLCD()->GetLY() >= LINES_PER_FRAME)
		{
			EMU::GetLCD()->Set_PPU_Mode(LCD_Mode::OAM);
			EMU::GetLCD()->SetLY(0);
		}

		line_ticks = 0;
	}
}

void PPU::TICK_HBLANK()
{
	if (line_ticks >= TICKS_PER_LINE)
	{
		EMU::GetLCD()->IncrementLY();

		if (EMU::GetLCD()->GetLY() >= YRES)
		{
			EMU::GetLCD()->Set_PPU_Mode(LCD_Mode::VBLANK);

			EMU::GetCPU()->RequestInterrupt(IntType::IT_VBlank);

			if (EMU::GetLCD()->Get_Int_Src_Enabled(LCDS_Int_Src::VBLANK))
			{
				EMU::GetCPU()->RequestInterrupt(IntType::IT_LCD_Stat);
			}

			current_frame++;

			//calc FPS...
			u32 end = EMU::GetWindow()->GetTicks();
			u32 frame_time = end - prev_frame_time;

			if (frame_time < target_frame_time)
			{
				EMU::GetCPU()->Sleep((target_frame_time - frame_time));
			}

			if (end - start_timer >= 1000)
			{
				u32 fps = frame_count;
				start_timer = end;
				frame_count = 0;

				printf("FPS: %d\n", fps);
			}

			frame_count++;
			prev_frame_time = EMU::GetWindow()->GetTicks();

		}
		else
		{
			EMU::GetLCD()->Set_PPU_Mode(LCD_Mode::OAM);
		}

		line_ticks = 0;
	}
}
