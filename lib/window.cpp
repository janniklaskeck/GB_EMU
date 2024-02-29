#pragma once

#include "window.h"
#include "common.h"
#include "emu.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include "ram.h"
#include "bus.h"

using namespace GB;

Window::Window()
{
	SDL_Init(SDL_INIT_VIDEO);
	printf("SDL INIT\n");
	TTF_Init();
	printf("TTF INIT\n");

	SDL_CreateWindowAndRenderer(mainWindowWidth, mainWindowHeight, 0, &sdlWindow, &sdlRenderer);

	constexpr int debugWidth = 16 * 8 * debugScale;
	constexpr int debugHeight = 32 * 8 * debugScale;

	SDL_CreateWindowAndRenderer(debugWidth, debugHeight, 0, &debug_sdlWindow, &debug_sdlRenderer);

	debug_sdlSurface = SDL_CreateRGBSurface(0, 
											debugWidth + (16 * debugScale), 
											debugHeight + (64 * debugScale), 
											32,
											0x00FF0000,
											0x0000FF00,
											0x000000FF,
											0xFF000000);

	debug_sdlTexture = SDL_CreateTexture(debug_sdlRenderer,
										 SDL_PIXELFORMAT_ABGR8888,
										 SDL_TEXTUREACCESS_STREAMING,
										 debugWidth + (16 * debugScale),
										 debugHeight + (64 * debugScale));

	int windowX, windowY;
	SDL_GetWindowPosition(sdlWindow, &windowX, &windowY);
	SDL_SetWindowPosition(debug_sdlWindow, windowX + mainWindowWidth + 10, windowY);
}

void Window::HandleEvents()
{
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent) > 0)
	{
		if (sdlEvent.type == SDL_WINDOWEVENT && sdlEvent.window.event == SDL_WINDOWEVENT_CLOSE)
		{
			EMU::GetEMU()->Shutdown();
		}
	}
}

void Window::Delay(u32 MS)
{
	SDL_Delay(MS);
}

void Window::UpdateWindow()
{
	HandleEvents();

	UpdateMainWindow();

	UpdateDebugWindow();
}

void Window::UpdateMainWindow()
{

}

void Window::UpdateDebugWindow()
{
	u16 xDraw = 0;
	u16 yDraw = 0;
	u16 tileNum = 0;
	
	SDL_Rect screenRect{};
	screenRect.w = debug_sdlSurface->w;
	screenRect.h = debug_sdlSurface->h;
	SDL_FillRect(debug_sdlSurface, &screenRect, 0xFF111111);

	for (u16 y = 0; y < 24; y++)
	{
		for (u16 x = 0; x < 16; x++)
		{
			const u16 xTile = xDraw + (x * debugScale);
			const u16 yTile = yDraw + (y * debugScale);
			drawTile(tileNum, xTile, yTile);
			xDraw += (8 * debugScale);
			tileNum++;
		}

		yDraw += (8 * debugScale);
		xDraw = 0;
	}

	SDL_UpdateTexture(debug_sdlTexture, nullptr, debug_sdlSurface->pixels, debug_sdlSurface->pitch);
	SDL_RenderClear(debug_sdlRenderer);
	SDL_RenderCopy(debug_sdlRenderer, debug_sdlTexture, nullptr, nullptr);
	SDL_RenderPresent(debug_sdlRenderer);
}

void Window::drawTile(u16 tileNum, u16 xDraw, u16 yDraw)
{
	constexpr std::array<u32, 4> tileColors = { 0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000 };

	SDL_Rect tileRect;
	
	for (u8 tileY = 0; tileY < 16; tileY++)
	{
		const u16 address = RAM_ADDR::VRAM_START + (tileNum * 16) + tileY;
		u8 b1 = EMU::GetBUS()->ReadByte(address);
		u8 b2 = EMU::GetBUS()->ReadByte(address + 1);
		for (i8 bit = 7; bit >= 0; bit--)
		{
			const u8 hi = ((bool)(b1 & (1 << bit))) << 1;
			const u8 lo = (bool)(b2 & (1 << bit));

			const u8 color = hi | lo;
			tileRect.x = xDraw + ((7 - bit) * debugScale);
			tileRect.y = yDraw + (tileY / 2 * debugScale);
			tileRect.w = debugScale;
			tileRect.h = debugScale;

			SDL_FillRect(debug_sdlSurface, &tileRect, tileColors[color]);
		}
	}

}
