#include "AssetStore.h"
#include "../Logger/Logger.h"
#include <SDL2/SDL_image.h>

AssetStore::AssetStore()
{
	Logger::Log("AssetStore constructor called!");
}

AssetStore::~AssetStore()
{
	ClearAssets();
	Logger::Log("AssetStore destructor called!");
}

void AssetStore::ClearAssets()
{
	for(auto texture: textures)
	{
		SDL_DestroyTexture(texture);
	}

	for(auto font: fonts)
	{
		TTF_CloseFont(font);
	}
}

void AssetStore::AddTexture(SDL_Renderer* renderer, TextureIdEnum assetId, const std::string& filePath)
{
	SDL_Surface* surface = IMG_Load(filePath.c_str());
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	textures[(int)assetId] = texture;
}

SDL_Texture* AssetStore::GetTexture(const TextureIdEnum assetId)
{
	return textures[(int)assetId];
}

void AssetStore::AddFont(const FontIdEnum assetId, const std::string& filePath, int fontSize)
{
	fonts[(int)assetId] = TTF_OpenFont(filePath.c_str(), fontSize);
}

TTF_Font* AssetStore::GetFont(const FontIdEnum assetId)
{
	return fonts[(int)assetId];
}
