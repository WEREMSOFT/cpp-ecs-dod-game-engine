#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>

const int FPS = 500;
const int MILLISECONDS_PER_FRAME = 1000 / FPS;

class Game
{
private:
	SDL_Window *window;
	SDL_Renderer *renderer;
	bool isRunning;
	int windowWidth;
	int windowHeight;

	glm::vec2 playerPosition;
	glm::vec2 playerVelocity;

	int millisecondsPreviousFrame = 0;

public:
	Game();
	~Game();
	void Initialize();
	void Setup();
	void Run();
	void Update();
	void ProcessInput();
	void Render();
	void Destroy();
};