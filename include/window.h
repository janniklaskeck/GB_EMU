#pragma once

#include "common.h"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Surface;

namespace GB
{
	constexpr int debugScale = 2;

	class Window
	{

	public:

		Window();

		void HandleEvents();

		void Delay(u32 MS);

		void UpdateWindow();

		u32 GetTicks() const;

	protected:

		void UpdateMainWindow();

		void UpdateDebugWindow();

	protected:

		void drawTile(u16 tileNum, u16 xDraw, u16 yDraw);

	private:

		SDL_Window* sdlWindow = nullptr;
		SDL_Renderer* sdlRenderer = nullptr;
		SDL_Texture* sdlTexture = nullptr;
		SDL_Surface* sdlSurface = nullptr;

		SDL_Window* debug_sdlWindow = nullptr;
		SDL_Renderer* debug_sdlRenderer = nullptr;
		SDL_Texture* debug_sdlTexture = nullptr;
		SDL_Surface* debug_sdlSurface = nullptr;

		u16 mainWindowWidth = 1024;
		u16 mainWindowHeight = 768;

	};

}
