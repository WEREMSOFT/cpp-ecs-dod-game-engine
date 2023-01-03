#pragma once
#include <SDL2/SDL.h>

class Game
{
private:
	SDL_Window *window;
	SDL_Renderer *renderer;
	bool isRunning;

public:
	Game();
	~Game();
	void Initialize();
	void Run();
	void Update();
	void ProcessInput();
	void Render();
	void Destroy();
};