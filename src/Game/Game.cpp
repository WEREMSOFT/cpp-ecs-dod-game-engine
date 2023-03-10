#include "Game.h"
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <sol/sol.hpp>
#include <string>
#include <imgui/imgui.h>
#include <imgui/imgui_sdl.h>
#include <imgui/imgui_impl_sdl2.h>

#include <stdio.h>

#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/KeyboardControllerComponent.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/TextLabelComponent.h"

#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../Systems/DebugRenderSystem.h"
#include "../Systems/DamageSystem.h"
#include "../Systems/KeyboardControllerSystem.h"
#include "../Systems/CameraMovementSystem.h"
#include "../Systems/ProjectileEmitSystem.h"
#include "../Systems/ProjectileLifecycleSystem.h"
#include "../Systems/RenderTextSystem.h"
#include "../Systems/RenderHealthBarSystem.h"
#include "../Systems/RenderGuiSystem.h"

#include "../EventBus/EventBus.h"
#include "../Events/KeyPressedEvent.h"


int Game::windowWidth;
int Game::windowHeight;
int Game::mapWidth;
int Game::mapHeight;

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

	if(TTF_Init() != 0)
	{
		Logger::Err("Error initializing SDL TTF.");
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

	// initialize dearImgui
	ImGui::CreateContext();
	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGuiSDL::Initialize(renderer, windowWidth, windowHeight);

	camera = {0, 0, windowWidth, windowHeight};
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
	assetStore->AddTexture(renderer, TextureIdEnum::TILEMAP_IMAGE, "./assets/tilemaps/jungle.png");

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
	registry->AddSystem<KeyboardControllerSystem>();
	registry->AddSystem<CameraMovementSystem>();
	registry->AddSystem<ProjectileEmitSystem>();
	registry->AddSystem<ProjectileLifecycleSystem>();
	registry->AddSystem<RenderTextSystem>();
	registry->AddSystem<RenderHealthBarSystem>();
	registry->AddSystem<RenderGuiSystem>();

	// Add assets to the asset store
	assetStore->AddTexture(renderer, TextureIdEnum::TANK_IMAGE, "./assets/images/tank-panther-right.png");
	assetStore->AddTexture(renderer, TextureIdEnum::TRUCK_IMAGE, "./assets/images/truck-ford-right.png");
	assetStore->AddTexture(renderer, TextureIdEnum::CHOPPER_IMAGE, "./assets/images/chopper-spritesheet.png");
	assetStore->AddTexture(renderer, TextureIdEnum::RADAR_IMAGE, "./assets/images/radar.png");
	assetStore->AddTexture(renderer, TextureIdEnum::BULLET_IMAGE, "./assets/images/bullet.png");
	assetStore->AddTexture(renderer, TextureIdEnum::TREE_IMAGE, "./assets/images/tree.png");
	
	// Add fonts to the asset store
	assetStore->AddFont(FontIdEnum::MAIN, "./assets/fonts/charriot.ttf", 16);
	assetStore->AddFont(FontIdEnum::MAIN_SMALL, "./assets/fonts/charriot.ttf", 10);

	auto currentMap = LoadTileMap();

	double scale = 2.;

	int y = 0;
	int x = 0;
	
	for(auto vec: currentMap)
	{
		x = 0;
		for(auto currentTile: vec)
		{
			auto srcRect = SDL_Rect{(currentTile % 10) * 32, (currentTile / 10) * 32, 32, 32};
			SDL_Rect destRect = {x * 32, y * 32, 32, 32};
			Entity tile = registry->CreateEntity();
			tile.Group(GroupsIdEnum::TILES);
			tile.AddComponent<TransformComponent>(glm::vec2(destRect.x * scale, destRect.y * scale), glm::vec2(scale, scale));
			tile.AddComponent<SpriteComponent>(TextureIdEnum::TILEMAP_IMAGE, 32, 32, 0, srcRect.x, srcRect.y);
			x++;
		}
		y++;
	}

	mapWidth = x * 32 * scale;
	mapHeight = y * 32 * scale;

	millisecondsPreviousFrame = SDL_GetTicks();
	Entity tank = registry->CreateEntity();
	tank.AddComponent<TransformComponent>(glm::vec2(500., 500.), glm::vec2(1., 1.), 0.);
	tank.AddComponent<RigidBodyComponent>(glm::vec2(20., 0.));
	tank.AddComponent<SpriteComponent>(TextureIdEnum::TANK_IMAGE, 32, 32, 1);
	tank.AddComponent<BoxColliderComponent>(32, 32);
	// tank.AddComponent<ProjectileEmitterComponent>(glm::vec2(100, 0), 1000, 5000, 10, false);
	tank.AddComponent<HealthComponent>(100);
	tank.Group(GroupsIdEnum::ENEMIES);

	Entity truck = registry->CreateEntity();
	truck.AddComponent<TransformComponent>(glm::vec2(200., 500.), glm::vec2(1., 1.), 0.);
	truck.AddComponent<RigidBodyComponent>(glm::vec2(10., 0.));
	truck.AddComponent<SpriteComponent>(TextureIdEnum::TRUCK_IMAGE, 32, 32, 2);
	truck.AddComponent<BoxColliderComponent>(32, 32);
	truck.AddComponent<ProjectileEmitterComponent>(glm::vec2(0, 100), 1000, 2000, 10, false);
	truck.AddComponent<HealthComponent>(100);
	truck.Group(GroupsIdEnum::ENEMIES);

	// TreeA
	Entity treeA = registry->CreateEntity();
	treeA.AddComponent<TransformComponent>(glm::vec2(600., 495.), glm::vec2(1., 1.), 0.);
	treeA.AddComponent<SpriteComponent>(TextureIdEnum::TREE_IMAGE, 16, 32, 2);
	treeA.AddComponent<BoxColliderComponent>(16, 32);
	treeA.Group(GroupsIdEnum::OBSTACLES);

	// TreeB
	Entity treeB = registry->CreateEntity();
	treeB.AddComponent<TransformComponent>(glm::vec2(400., 495.), glm::vec2(1., 1.), 0.);
	treeB.AddComponent<SpriteComponent>(TextureIdEnum::TREE_IMAGE, 16, 32, 2);
	treeB.AddComponent<BoxColliderComponent>(16, 32);
	treeB.Group(GroupsIdEnum::OBSTACLES);

	Entity chopper = registry->CreateEntity();
	chopper.AddComponent<TransformComponent>(glm::vec2(240., 111.), glm::vec2(1., 1.), 0.);
	chopper.AddComponent<SpriteComponent>(TextureIdEnum::CHOPPER_IMAGE, 32, 32, 3);
	chopper.AddComponent<RigidBodyComponent>(glm::vec2(0.));
	chopper.AddComponent<AnimationComponent>(2, 7);
	chopper.AddComponent<BoxColliderComponent>(32, 32);
	chopper.AddComponent<CameraFollowComponent>();
	chopper.AddComponent<HealthComponent>(100);
	chopper.AddComponent<ProjectileEmitterComponent>(glm::vec2(200, 200), 0, 10000, 10, true);
	chopper.Tag(TagsIdEnum::PLAYER);
	
	{
		int velocity = 80;
		chopper.AddComponent<KeyboardControllerComponent>(
			glm::vec2( 0, -velocity),
			glm::vec2( velocity, 0),
			glm::vec2(   0, velocity),
			glm::vec2(-velocity, 0));
	}


	Entity radar = registry->CreateEntity();
	radar.AddComponent<TransformComponent>(glm::vec2(windowWidth - 74, 10), glm::vec2(1., 1.));
	radar.AddComponent<RigidBodyComponent>(glm::vec2(0., 0.));
	radar.AddComponent<SpriteComponent>(TextureIdEnum::RADAR_IMAGE, 64, 64, 13, 0, 0, true);
	radar.AddComponent<AnimationComponent>(8, 15, true);

	SDL_Color colorGreen = {0, 255, 0};

	Entity label = registry->CreateEntity();
	label.AddComponent<TextLabelComponent>(glm::vec2(windowWidth / 2 - 40, 10), "CHOPPER 1.0", FontIdEnum::MAIN, colorGreen, true);
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
		// Handle Imgui events
		ImGui_ImplSDL2_ProcessEvent(&event);
		ImGuiIO& io = ImGui::GetIO();

		int mouseX, mouseY;

		const int buttons = SDL_GetMouseState(&mouseX, &mouseY);
		io.MousePos = ImVec2(mouseX, mouseY);
		io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
		io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);

		// Handle SDL events
		switch (event.type)
		{
		case SDL_QUIT:
			isRunning = false;
			break;
		case SDL_KEYDOWN:
			eventBus->EmitEvent<KeyPressedEvent>(event.key.keysym.sym);
			
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				isRunning = false;
				break;
			}
			if (event.key.keysym.sym == SDLK_d)
			{
				isDebugMode = !isDebugMode;
				break;
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
	registry->GetSystem<MovementSystem>().SubscribeToEvents(eventBus);
	registry->GetSystem<DamageSystem>().SubscribeToEvents(eventBus);
	registry->GetSystem<KeyboardControllerSystem>().SubscribeToEvents(eventBus);
	registry->GetSystem<ProjectileEmitSystem>().SubscribeToEvents(eventBus);
	
	registry->Update();

	registry->GetSystem<MovementSystem>().Update(deltaTime);
	registry->GetSystem<AnimationSystem>().Update();
	registry->GetSystem<CollisionSystem>().Update(eventBus);
	registry->GetSystem<CameraMovementSystem>().Update(camera);
	registry->GetSystem<ProjectileEmitSystem>().Update(registry);
	registry->GetSystem<ProjectileLifecycleSystem>().Update();
}

void Game::Render()
{
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
	SDL_RenderClear(renderer);
	// Call all the systems that requires an object
	registry->GetSystem<RenderSystem>().Update(renderer, assetStore, camera);
	registry->GetSystem<RenderTextSystem>().Update(renderer, assetStore, camera);
	registry->GetSystem<RenderHealthBarSystem>().Update(renderer, assetStore, camera);	
	if(isDebugMode)
	{
		registry->GetSystem<RenderGuiSystem>().Update(registry, camera);
		registry->GetSystem<DebugRenderSystem>().Update(renderer, camera);
	}

	SDL_RenderPresent(renderer);
}

void Game::Destroy()
{
	ImGuiSDL::Deinitialize();
	ImGui::DestroyContext();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

Game::~Game()
{
	Logger::Log("Destroying game instance");
}