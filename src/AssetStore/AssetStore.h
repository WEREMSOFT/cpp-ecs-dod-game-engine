#pragma once

#include <map>
#include <string>
#include <SDL2/SDL.h> 
#include <SDL2/SDL_ttf.h>

enum class TextureIdEnum
{
	TANK_IMAGE,
	TRUCK_IMAGE,
	CHOPPER_IMAGE,
	TILEMAP_IMAGE,
	RADAR_IMAGE,
	BULLET_IMAGE,
	COUNT,
	NONE
};

enum class FontIdEnum
{
	MAIN,
	MAIN_SMALL,
	COUNT,
	NONE
};

class AssetStore
{
	private:
		SDL_Texture* textures[(unsigned int)TextureIdEnum::COUNT];
		TTF_Font* fonts[(unsigned int)FontIdEnum::COUNT];

	public:
		AssetStore();
		~AssetStore();

		void ClearAssets();

		void AddTexture(SDL_Renderer* renderer, TextureIdEnum assetId, const std::string& filePath);
		SDL_Texture* GetTexture(const TextureIdEnum asstId);

		void AddFont(const FontIdEnum assetId, const std::string& filePath, int fontSize);
		TTF_Font* GetFont(const FontIdEnum assetId);
};