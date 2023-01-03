#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>

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