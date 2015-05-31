// TiledMap.cpp

// SDL Includes
#include <SDL2\SDL.h>
#include <SDL2\SDL_image.h>
// Project Includes
#include "Globals.hpp"
#include "TiledMap.hpp"
// JSON
#include <json\json.h>
// STL Includes
#include <cstdlib>
#include <fstream>
#include <iostream>

TiledMap::TiledMap()
{
}

TiledMap::~TiledMap()
{
	m_Data.clear();
	m_BitMasks.clear();
}

bool TiledMap::Load(const std::string& p_Filename)
{
	// Open the file.
	std::ifstream file;
	file.open(p_Filename);
	if (!file.is_open())
		return false;

	std::string tilesetFilename, bitmaskFilename;
	int bitWidth, bitHeight, bitSize;
	bool isLoaded = false;

	// Check file extension.
	std::string ext = p_Filename.substr(p_Filename.find_last_of('.') + 1);
	if (ext == "txt")
	{
		int version;

		file >> version;
		file >> m_Width >> m_Height;
		file >> tilesetFilename;
		file >> m_Tileset.X >> m_Tileset.Y >> m_Tileset.Size;
		file >> bitmaskFilename;
		file >> bitWidth >> bitHeight >> bitSize;

		m_Data.resize(m_Width, std::vector<Tile>(m_Height));

		// Load tile values.
		for (int y = 0; y < m_Height; ++y)
		{
			for (int x = 0; x < m_Width; ++x)
			{
				int tex, col, ori;
				file >> tex >> col >> ori;

				Tile& tile = m_Data[x][y];
				tile.TextureID = tex;
				tile.CollisionID = col;
				tile.Orientation = static_cast<SAGE::Orientation>(ori);
			}
		}

		isLoaded = true;
	}
	else if (ext == "json")
	{
		Json::Value root;
		Json::Reader reader;
		if (reader.parse(file, root, false))
		{
			int version = root["version"].asInt();
			m_Width = root["width"].asInt();
			m_Height = root["height"].asInt();
			const Json::Value tileset = root["tileset"];
			const Json::Value bitset = root["bitset"];
			const Json::Value data = root["data"];

			m_Tileset.X = tileset.get("x", 0).asInt();
			m_Tileset.Y = tileset.get("y", 0).asInt();
			m_Tileset.Size = tileset.get("size", 0).asInt();
			tilesetFilename = tileset.get("path", "").asString();

			bitWidth = bitset.get("x", 0).asInt();
			bitHeight = bitset.get("y", 0).asInt();
			bitSize = bitset.get("size", 0).asInt();
			bitmaskFilename = bitset.get("path", "").asString();

			m_Data.resize(m_Width, std::vector<Tile>(m_Height));

			// Load tile values.
			for (int y = 0; y < m_Height; ++y)
			{
				const Json::Value row = data[y];

				for (int x = 0; x < m_Width; ++x)
				{
					const Json::Value item = row[x];
					int index = 0;

					Tile& tile = m_Data[x][y];
					tile.TextureID = item[index++].asInt();
					tile.CollisionID = item[index++].asInt();
					tile.Orientation = static_cast<SAGE::Orientation>(item[index].asInt());
				}
			}

			isLoaded = true;
		}
		else
		{
			SDL_Log("[TiledMap::Load] Failed to parse JSON file.");
		}
	}
	else
	{
		SDL_Log("[TiledMap::Load] Unsupported map file format: %s", ext);
	}

	file.close();

	if (isLoaded)
	{
		// Load tileset texture;
		if (!m_Tileset.Texture.Load(tilesetFilename.c_str(), HGF::Interpolation::Nearest, HGF::Wrapping::ClampToBorder))
			return false;

		// Load bitmask texture.
		HGF::Texture bitmaskTexture;
		if (!bitmaskTexture.Load(bitmaskFilename.c_str()))
			return false;

		// Establish bitmask count.
		m_BitMaskCount = bitWidth * bitHeight;
		m_BitMasks.resize(m_BitMaskCount);

		// Read the bitmask pixels.
		for (int i = 0; i < m_BitMaskCount; ++i)
		{
			m_BitMasks[i].Resize(m_Tileset.Size, m_Tileset.Size);

			for (int px = 0; px < m_Tileset.Size; ++px)
			{
				for (int py = 0; py < m_Tileset.Size; ++py)
				{
					Uint8 r, g, b, a;
					bitmaskTexture.GetColor(i * m_Tileset.Size + px, py, r, g, b, a);

					m_BitMasks[i].SetBit(px, py, (r == 0 && g == 0 && b == 0));
				}
			}
		}

		// Assign attibutes from values.
		for (int y = 0; y < m_Height; ++y)
		{
			for (int x = 0; x < m_Width; ++x)
			{
				Tile& tile = m_Data[x][y];

				// Assign edge type depending on collision. (Temporary)
				switch (tile.CollisionID)
				{
				case 0:
					for (int i = 0; i < 4; ++i)
						tile.Edges[i] = EdgeMask::Solid;
					break;
				case 1:
				case 3:
					tile.Edges[EdgeSide::Top] = EdgeMask::Interesting;
					tile.Edges[EdgeSide::Bottom] = EdgeMask::Solid;
					tile.Edges[EdgeSide::Left] = EdgeMask::Interesting;
					tile.Edges[EdgeSide::Right] = EdgeMask::Solid;
					break;
				case 2:
				case 4:
					tile.Edges[EdgeSide::Top] = EdgeMask::Interesting;
					tile.Edges[EdgeSide::Bottom] = EdgeMask::Solid;
					tile.Edges[EdgeSide::Left] = EdgeMask::Interesting;
					tile.Edges[EdgeSide::Right] = EdgeMask::Interesting;
					break;
				default:
					for (int i = 0; i < 4; ++i)
						tile.Edges[i] = EdgeMask::Empty;
					break;
				}

				// Flip edges for orientation.
				if ((tile.Orientation & SAGE::Orientation::FlipVertical) == SAGE::Orientation::FlipVertical)
					std::swap(tile.Edges[EdgeSide::Top], tile.Edges[EdgeSide::Bottom]);
				if ((tile.Orientation & SAGE::Orientation::FlipHorizontal) == SAGE::Orientation::FlipHorizontal)
					std::swap(tile.Edges[EdgeSide::Left], tile.Edges[EdgeSide::Right]);
			}
		}

		// Resolve collision edges.
		for (int y = 0; y < m_Height; ++y)
		{
			for (int x = 0; x < m_Width; ++x)
			{
				Tile& tile = m_Data[x][y];

				if (x + 1 != m_Width)
				{
					Tile& tileHorz = m_Data[x + 1][y];

					if ((tile.Edges[EdgeSide::Right] == EdgeMask::Solid || tile.Edges[EdgeSide::Right] == EdgeMask::Interesting) &&
						(tileHorz.Edges[EdgeSide::Left] == EdgeMask::Solid || tileHorz.Edges[EdgeSide::Left] == EdgeMask::Interesting))
					{
						tile.Edges[EdgeSide::Right] = EdgeMask::Empty;
						tileHorz.Edges[EdgeSide::Left] = EdgeMask::Empty;
					}
				}
				if (y + 1 != m_Height)
				{
					Tile& tileVert = m_Data[x][y + 1];

					if ((tile.Edges[EdgeSide::Bottom] == EdgeMask::Solid || tile.Edges[EdgeSide::Bottom] == EdgeMask::Interesting) &&
						(tileVert.Edges[EdgeSide::Top] == EdgeMask::Solid || tileVert.Edges[EdgeSide::Top] == EdgeMask::Interesting))
					{
						tile.Edges[EdgeSide::Bottom] = EdgeMask::Empty;
						tileVert.Edges[EdgeSide::Top] = EdgeMask::Empty;
					}
				}
			}
		}
	}

	return isLoaded;
}

void TiledMap::Raycast(const HGF::Vector2& p_Position, const HGF::Vector2& p_Direction, int p_LayerMasks, RaycastData& p_RaycastData) const
{
	int x = (int)std::roundf(p_Position.X);
	int y = (int)std::roundf(p_Position.Y);
	int tileX = x / m_Tileset.Size;
	int tileY = y / m_Tileset.Size;
	int pixelX = x % m_Tileset.Size;
	int pixelY = y % m_Tileset.Size;

	float top = 0.0f;
	float bottom = (float)(m_Height * m_Tileset.Size);
	float right = (float)(m_Width * m_Tileset.Size);
	float left = 0.0f;

	p_RaycastData.StartPosition = p_Position;
	p_RaycastData.Direction = p_Direction;

	// Capture outside cases.
	if ((p_Direction == HGF::Vector2::Up && (p_Position.Y < top || p_Position.X < left || p_Position.X > right)) ||
		(p_Direction == HGF::Vector2::Down && (p_Position.Y > bottom || p_Position.X < left || p_Position.X > right)) ||
		(p_Direction == HGF::Vector2::Left && (p_Position.X < left || p_Position.Y < top || p_Position.Y > bottom)) ||
		(p_Direction == HGF::Vector2::Right && (p_Position.X > right || p_Position.Y < top || p_Position.Y > bottom)))
	{
		p_RaycastData.IsValid = false;
		return;
	}

	Tile tile;
	if (p_Direction == HGF::Vector2::Up)
	{
		for (int ty = tileY; ty >= 0; --ty)
		{
			if (TryGetTile(tileX, ty, tile))
			{
				if (IsEnterable(tile, EdgeSide::Bottom, p_LayerMasks))
				{
					for (int py = m_Tileset.Size - 1; py >= 0; --py)
					{
						if (!IsTraversable(tile, pixelX, py))
						{
							p_RaycastData.EndPosition = HGF::Vector2(p_Position.X, (float)(ty * m_Tileset.Size + py));
							p_RaycastData.Distance = HGF::Vector2::Distance(p_RaycastData.StartPosition, p_RaycastData.EndPosition);
							return;
						}
					}
				}
			}
		}
		p_RaycastData.EndPosition = HGF::Vector2(p_Position.X, top);
		p_RaycastData.Distance = HGF::Vector2::Distance(p_RaycastData.StartPosition, p_RaycastData.EndPosition);
	}
	else if (p_Direction == HGF::Vector2::Down)
	{
		for (int ty = tileY; ty < m_Height; ++ty)
		{
			if (TryGetTile(tileX, ty, tile))
			{
				if (IsEnterable(tile, EdgeSide::Top, p_LayerMasks))
				{
					for (int py = 0; py < m_Tileset.Size; ++py)
					{
						if (!IsTraversable(tile, pixelX, py))
						{
							p_RaycastData.EndPosition = HGF::Vector2(p_Position.X, (float)(ty * m_Tileset.Size + py));
							p_RaycastData.Distance = HGF::Vector2::Distance(p_RaycastData.StartPosition, p_RaycastData.EndPosition);
							return;
						}
					}
				}
			}
		}
		p_RaycastData.EndPosition = HGF::Vector2(p_Position.X, bottom);
		p_RaycastData.Distance = HGF::Vector2::Distance(p_RaycastData.StartPosition, p_RaycastData.EndPosition);
	}
	else if (p_Direction == HGF::Vector2::Left)
	{
		for (int tx = tileX; tx >= 0; --tx)
		{
			if (TryGetTile(tx, tileY, tile))
			{
				if (IsEnterable(tile, EdgeSide::Right, p_LayerMasks))
				{
					for (int px = m_Tileset.Size - 1; px >= 0; --px)
					{
						if (!IsTraversable(tile, px, pixelY))
						{
							p_RaycastData.EndPosition = HGF::Vector2((float)(tx * m_Tileset.Size + px), p_Position.Y);
							p_RaycastData.Distance = HGF::Vector2::Distance(p_RaycastData.StartPosition, p_RaycastData.EndPosition);
							return;

						}
					}
				}
			}
		}
		p_RaycastData.EndPosition = HGF::Vector2(left, p_Position.Y);
		p_RaycastData.Distance = HGF::Vector2::Distance(p_RaycastData.StartPosition, p_RaycastData.EndPosition);
		p_RaycastData.IsValid = false;
	}
	else if (p_Direction == HGF::Vector2::Right)
	{
		for (int tx = tileX; tx < m_Width; ++tx)
		{
			if (TryGetTile(tx, tileY, tile))
			{
				if (IsEnterable(tile, EdgeSide::Left, p_LayerMasks))
				{
					for (int px = 0; px < m_Tileset.Size; ++px)
					{
						if (!IsTraversable(tile, px, pixelY))
						{
							p_RaycastData.EndPosition = HGF::Vector2((float)(tx * m_Tileset.Size + px), p_Position.Y);
							p_RaycastData.Distance = HGF::Vector2::Distance(p_RaycastData.StartPosition, p_RaycastData.EndPosition);
							return;
						}
					}
				}
			}
		}
		p_RaycastData.EndPosition = HGF::Vector2(right, p_Position.Y);
		p_RaycastData.Distance = HGF::Vector2::Distance(p_RaycastData.StartPosition, p_RaycastData.EndPosition);
	}
}

void TiledMap::Update(float pDeltaTime)
{
}

void TiledMap::Render(SAGE::SpriteBatch& p_SpriteBatch)
{
	Tile tile;
	for (int y = 0; y < m_Height; ++y)
	{
		for (int x = 0; x < m_Width; ++x)
		{
			if (TryGetTile(x, y, tile))
			{
				HGF::Vector2 position(x * m_Tileset.Size, y * m_Tileset.Size);

				HGF::Rectangle source;
				source.X = (tile.TextureID % m_Tileset.X) * m_Tileset.Size;
				source.Y = (tile.TextureID / m_Tileset.X)  * m_Tileset.Size;
				source.Width = m_Tileset.Size;
				source.Height = m_Tileset.Size;

				p_SpriteBatch.Draw(m_Tileset.Texture, position, source, HGF::Color::White, HGF::Vector2::Zero, 0.0f, HGF::Vector2::One, tile.Orientation);
			}
		}
	}
}

void TiledMap::Render(SAGE::GeometryBatch& p_GeometryBatch)
{
	Tile tile;
	for (int y = 0; y < m_Height; ++y)
	{
		for (int x = 0; x < m_Width; ++x)
		{
			if (TryGetTile(x, y, tile))
			{
				for (int i = 0; i < 4; ++i)
				{
					if (tile.Edges[i] != EdgeMask::Empty)
					{
						HGF::Color color(tile.Edges[i] == EdgeMask::Interesting ? 0.0f : 0.8f, 0.0f, tile.Edges[i] == EdgeMask::Interesting ? 0.8f : 0.0f);

						switch (i)
						{
							case EdgeSide::Top:
								p_GeometryBatch.DrawLine(HGF::Vector2(x * m_Tileset.Size, y * m_Tileset.Size), HGF::Vector2((x + 1) * m_Tileset.Size, y * m_Tileset.Size), color);
								break;
							case EdgeSide::Bottom:
								p_GeometryBatch.DrawLine(HGF::Vector2(x * m_Tileset.Size, (y + 1) * m_Tileset.Size), HGF::Vector2((x + 1) * m_Tileset.Size, (y + 1) * m_Tileset.Size), color);
								break;
							case EdgeSide::Left:
								p_GeometryBatch.DrawLine(HGF::Vector2(x * m_Tileset.Size, y * m_Tileset.Size), HGF::Vector2(x * m_Tileset.Size, (y + 1) * m_Tileset.Size), color);
								break;
							case EdgeSide::Right:
								p_GeometryBatch.DrawLine(HGF::Vector2((x + 1) * m_Tileset.Size, y * m_Tileset.Size), HGF::Vector2((x + 1) * m_Tileset.Size, (y + 1) * m_Tileset.Size), color);
								break;
						}
					}
				}
			}
		}
	}
}

bool TiledMap::TryGetTile(int pX, int pY, Tile& p_Tile) const
{
	if (pX < 0 || pY < 0 || pX >= m_Width || pY >= m_Height)
		return false;

	p_Tile = m_Data[pX][pY];

	return p_Tile.TextureID >= 0;
}

bool TiledMap::IsEnterable(const Tile& p_Tile, EdgeSide p_Edge, int p_Mask) const
{
	if (p_Mask == EdgeMask::Interesting)
	{
		return p_Tile.Edges[p_Edge] == EdgeMask::Solid || p_Tile.Edges[p_Edge] == EdgeMask::Interesting;
	}
	else if (p_Mask == EdgeMask::Solid)
	{
		return p_Tile.Edges[p_Edge] == EdgeMask::Solid;
	}

	return false;
}

bool TiledMap::IsTraversable(const Tile& p_Tile, int p_PixelX, int p_PixelY) const
{
	if (p_PixelX < 0 || p_PixelY < 0 || p_PixelX >= m_Tileset.Size || p_PixelY >= m_Tileset.Size)
		return true;

	if (p_Tile.CollisionID < 0)
		return true;

	bool horzFlip = (p_Tile.Orientation & SAGE::Orientation::FlipHorizontal) == SAGE::Orientation::FlipHorizontal;
	bool vertFlip = (p_Tile.Orientation & SAGE::Orientation::FlipVertical) == SAGE::Orientation::FlipVertical;

	return !m_BitMasks[p_Tile.CollisionID].GetBit(horzFlip ? m_Tileset.Size - p_PixelX - 1 : p_PixelX, vertFlip ? m_Tileset.Size - p_PixelY - 1 : p_PixelY);
}
