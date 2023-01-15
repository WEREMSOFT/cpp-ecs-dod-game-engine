#include "Game.h"
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <sol/sol.hpp>
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Systems/MovementSystem.h"

Game::Game()
{
	Logger::Log("Creating game instance");
	Logger::Err("This is a fake error");
	isRunning = false;

	registry = std::make_unique<Registry>();
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

void Game::Setup()
{
	registry->AddSystem<MovementSystem>();

	millisecondsPreviousFrame = SDL_GetTicks();
	Entity tank = registry->CreateEntity();

	tank.AddComponent<TransformComponent>(glm::vec2(10., 30.), glm::vec2(1., 1.), 0.);
	tank.AddComponent<RigidBodyComponent>(glm::vec2(50., 50.));
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

	// TODO:
	registry->GetSystem<MovementSystem>().Update(deltaTime);
	// CollisionSystem.Update(deltaTime);
	// DamageSystem.Update(deltaTime);

	registry->Update();
}

void Game::Render()
{
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
	SDL_RenderClear(renderer);
	// TODO: Render Game Objects
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