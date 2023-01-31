#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include "../ECS/ECS.h"
#include "../Logger/Logger.h"
#include "../AssetStore/AssetStore.h"
#include "../EventBus/EventBus.h"

const int FPS = 60;
const int MILLISECONDS_PER_FRAME = 1000 / FPS;

class Game
{
private:
	SDL_Window *window;
	SDL_Renderer *renderer;

	std::unique_ptr<Registry> registry;
	std::unique_ptr<AssetStore> assetStore;
	std::unique_ptr<EventBus> eventBus;


	SDL_Rect camera;
	bool isDebugMode;
	bool isRunning;


	int millisecondsPreviousFrame = 0;

	std::vector<std::vector<int>> LoadTileMap();

public:
	static int windowWidth;
	static int windowHeight;
	static int mapWidth;
	static int mapHeight;

	Game();
	~Game();
	void Initialize();
	void Setup();
	void Run();
	void Update();
	void ProcessInput();
	void Render();
	void Destroy();
	void LoadLevel(int level);
};