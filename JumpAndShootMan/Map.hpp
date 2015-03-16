#ifndef __MAP_HPP__
#define __MAP_HPP__

struct Tileset
{
	unsigned int TextureID;
	int X;
	int Y;
	int Size;
};

struct Tile
{
	int TextureID;
	int CollisionID;
};

class Map
{
	public:
		Map();
		~Map();

		Tileset Tileset;
		int Width;
		int Height;
		Tile** Data;
		int BitMaskCount;
		bool*** BitMasks;
};

#endif