#include "Game.h"
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <sol/sol.hpp>
#include <string>

#include <stdio.h>

#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/BoxColliderComponent.h"

#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../Systems/DebugRenderSystem.h"
#include "../Systems/DamageSystem.h"

Game::Game()
{
	Logger::Log("Creating game instance");
	Logger::Err("This is a fake error");
	isRunning = false;
	isDebugMode = false;

	registry = std::make_unique<Registry>();
	assetStore = std::make_unique<AssetStore>();

	eventBus = std::make_unique<EventBus>();
}

void Game::Initialize()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		Logger::Err("Error initializing SDL");
		return;
	}

	SDL_DisplayMode displayMode;
	SDL_GetCurrentDisplayMode(0, &displayMode);

	windowWidth = 800;	// displayMode.w;
	windowHeight = 600; // displayMode.h;

	window = SDL_CreateWindow(
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		windowWidth,
		windowHeight,
		SDL_WINDOW_BORDERLESS);

	if (!window)
	{
		Logger::Err("Error creating window");
		return;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer)
	{
		Logger::Err("Error creating renderer");
		return;
	}

	// SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	isRunning = true;
}

std::vector<int> split(std::string stringToSplit)
{
	auto size = stringToSplit.size();
	char* buffer = new char[size + 1];
	memcpy(buffer, stringToSplit.c_str(), size + 1);
	char* token = strtok(buffer, ",");
	std::vector<int> returnValue;
	auto lineContent = std::string(buffer);

	while(token != NULL)
	{
		returnValue.emplace_back(atoi(token));
		token = strtok(NULL, ",\n");
	}

	delete buffer;
	return returnValue;
}

std::vector<std::vector<int>> Game::LoadTileMap()
{
	std::vector<std::vector<int>> returnValue;
	assetStore->AddTexture(renderer, "jungle-image", "./assets/tilemaps/jungle.png");

	FILE* fp = fopen("./assets/tilemaps/jungle.map", "r");

	if(fp == NULL)
	{
		Logger::Err(std::string("Errror al abrir archivo ") + std::string("./assets/tilemaps/jungle.map"));
		exit(-1);
	}

	char lineContents[100] = { 0 };

	while(fgets(lineContents, 100, fp))
	{
		returnValue.push_back(split(std::string(lineContents)));
	}

	fclose(fp);

	return returnValue;
}

void Game::LoadLevel(int level)
{
	registry->AddSystem<MovementSystem>();
	registry->AddSystem<RenderSystem>();
	registry->AddSystem<AnimationSystem>();
	registry->AddSystem<CollisionSystem>();
	registry->AddSystem<DebugRenderSystem>();
	registry->AddSystem<DamageSystem>();

	// Add assets to the asset store
	assetStore->AddTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
	assetStore->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
	assetStore->AddTexture(renderer, "chopper-image", "./assets/images/chopper.png");
	assetStore->AddTexture(renderer, "radar-image", "./assets/images/radar.png");

	auto currentMap = LoadTileMap();

	double scale = 1.;

	int y = 0;

	for(auto vec: currentMap)
	{
		int x = 0;
		for(auto currentTile: vec)
		{
			auto srcRect = SDL_Rect{(currentTile % 10) * 32, (currentTile / 10) * 32, 32, 32};
			SDL_Rect destRect = {x * 32, y * 32, 32, 32};
			Entity tile = registry->CreateEntity();
			tile.AddComponent<TransformComponent>(glm::vec2(destRect.x * scale, destRect.y * scale), glm::vec2(scale, scale));
			tile.AddComponent<SpriteComponent>("jungle-image", 32, 32, 0, srcRect.x, srcRect.y);
			x++;
		}
		y++;
	}


	millisecondsPreviousFrame = SDL_GetTicks();
	Entity tank = registry->CreateEntity();
	tank.AddComponent<TransformComponent>(glm::vec2(200., 50.), glm::vec2(1., 1.), 0.);
	tank.AddComponent<RigidBodyComponent>(glm::vec2(-40., 0.));
	tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 1);
	tank.AddComponent<BoxColliderComponent>(32, 32);

	Entity truck = registry->CreateEntity();
	truck.AddComponent<TransformComponent>(glm::vec2(10., 50.), glm::vec2(1., 1.), 0.);
	truck.AddComponent<RigidBodyComponent>(glm::vec2(44., 0.));
	truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 2);
	truck.AddComponent<BoxColliderComponent>(32, 32);

	Entity chopper = registry->CreateEntity();
	chopper.AddComponent<TransformComponent>();
	chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 3);
	chopper.AddComponent<RigidBodyComponent>(glm::vec2(10., 0.));
	chopper.AddComponent<AnimationComponent>(2, 7);

	Entity radar = registry->CreateEntity();
	radar.AddComponent<TransformComponent>(glm::vec2(windowWidth - 74, 10), glm::vec2(1., 1.));
	radar.AddComponent<RigidBodyComponent>(glm::vec2(0., 0.));
	radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 13);
	radar.AddComponent<AnimationComponent>(8, 15, true);
}

void Game::Setup()
{
	LoadLevel(1);
}

void Game::Run()
{
	Setup();
	while (isRunning)
	{
		ProcessInput();
		Update();
		Render();
	}
}

void Game::ProcessInput()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			isRunning = false;
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				isRunning = false;
			}
			if (event.key.keysym.sym == SDLK_d)
			{
				isDebugMode = !isDebugMode;
			}
			break;
		}
	}
}

void Game::Update()
{
	int currentTicks = SDL_GetTicks();
	int timeToWait = MILLISECONDS_PER_FRAME - currentTicks + millisecondsPreviousFrame;
	if (timeToWait > 0 && timeToWait < MILLISECONDS_PER_FRAME)
		SDL_Delay(timeToWait);

	float deltaTime = (SDL_GetTicks() - millisecondsPreviousFrame) / 1000.f;
	millisecondsPreviousFrame = SDL_GetTicks();

	// Reset event handlers for the current frame
	eventBus->Reset();

	// Perform the subscription of events for all systems
	registry->GetSystem<DamageSystem>().SubscribeToEvents(eventBus);
	
	registry->Update();

	// TODO:
	registry->GetSystem<MovementSystem>().Update(deltaTime);
	registry->GetSystem<AnimationSystem>().Update();
	registry->GetSystem<CollisionSystem>().Update(eventBus);
}

void Game::Render()
{
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
	SDL_RenderClear(renderer);
	// Call all the systems that requires an object
	registry->GetSystem<RenderSystem>().Update(renderer, assetStore);
	
	if(isDebugMode)
		registry->GetSystem<DebugRenderSystem>().Update(renderer);

	SDL_RenderPresent(renderer);
}

void Game::Destroy()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

Game::~Game()
{
	Logger::Log("Destroying game instance");
}