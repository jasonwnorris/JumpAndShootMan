#ifndef __MAP_HPP__
#define __MAP_HPP__

#include "Texture.hpp"

#include <string>

struct Tileset
{
	HGF::Texture Texture;
	int X;
	int Y;
	int Size;
};

struct Tile
{
	int TextureID;
	int CollisionID;
};

class Renderer;
class Map
{
	public:
		Map();
		~Map();

		bool Load(const std::string& pFilename);
		bool IsTraversable(int pX, int pY);
		bool Render(const Renderer& pRenderer);

		Tileset Tileset;
		int Width;
		int Height;
		Tile** Data;
		int BitMaskCount;
		bool*** BitMasks;
};

#endif