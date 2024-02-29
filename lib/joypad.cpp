#include "joypad.h"

#include <memory>

using namespace GB;


Joypad* Joypad::GetJoypad()
{
	static std::unique_ptr<Joypad> joypad(new Joypad);
	return joypad.get();
}

void Joypad::WriteState(u8 newValue)
{
	GetJoypad()->state = newValue;
}

u8 Joypad::ReadState()
{
	return GetJoypad()->state;
}

bool Joypad::IsJoypad_Addr(u16 address)
{
	return false; //address == 0xFF00;
}
