#pragma once
#include <SDL2/SDL.h>
#include "../EventBus/Event.h"

class KeyPressedEvent: public Event
{
	public:
		SDL_Keycode code;
		KeyPressedEvent(SDL_Keycode code): code(code){}
};