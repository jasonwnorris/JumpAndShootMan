#include "World.hpp"

World::World()
{

}

World::~World()
{

}

void World::HandleWorldCollisionViaPoints(Player& pPlayer, const Map& pMap)
{
	/*
	HitData hitData;
	bool hitHorz = false;
	bool hitVert = false;

	// left collision
	for (int i = 3; i <= 5; ++i)
	{
		// 3 = MRK_LEFT_CENTER
		// 4 = MRK_LEFT_TOP
		// 5 = MRK_LEFT_BOTTOM
		int start = (int)(pPlayer.Position.X + pPlayer.Dimensions.X / 2.0f);
		int finish = (int)(pPlayer.Position.X + pPlayer.Velocity.X + pPlayer.RaycastInfos[i].Position.X);
		int y = (int)(pPlayer.Position.Y + pPlayer.RaycastInfos[i].Position.Y);
		for (int x = start; x >= finish; --x)
		{
			if (CheckPoint(pMap, x, y, hitData))
			{
				if (pMap.Data[hitData.TileX][hitData.TileY].CollisionID == 0)
				{
					pPlayer.Position.X = (float)(x - 3) + pPlayer.RaycastInfos[i].Position.X;
					pPlayer.Velocity.X = 0.0f;
					hitHorz = true;
					break;
				}
			}
		}
		if (hitHorz)
			break;
	}

	// right collision
	for (int i = 6; i <= 8; ++i)
	{
		// 6 = MRK_RIGHT_CENTER
		// 7 = MRK_RIGHT_TOP
		// 8 = MRK_RIGHT_BOTTOM
		int start = (int)(pPlayer.Position.X + pPlayer.Dimensions.X / 2.0f);
		int finish = (int)(pPlayer.Position.X + pPlayer.Velocity.X + pPlayer.RaycastInfos[i].Position.X);
		int y = (int)(pPlayer.Position.Y + pPlayer.RaycastInfos[i].Position.Y);
		for (int x = start; x <= finish; ++x)
		{
			if (CheckPoint(pMap, x, y, hitData))
			{
				if (pMap.Data[hitData.TileX][hitData.TileY].CollisionID == 0)
				{
					pPlayer.Position.X = (float)x - pPlayer.RaycastInfos[i].Position.X;
					pPlayer.Velocity.X = 0.0f;
					hitHorz = true;
					break;
				}
			}
		}
		if (hitHorz)
			break;
	}

	// progress with horizontal movement if no collision
	if (!hitHorz)
		pPlayer.Position.X += pPlayer.Velocity.X;

	// bottom collision
	for (int i = 0; i <= 2; ++i)
	{
		// 0 = MRK_HOTSPOT
		// 1 = MRK_FOOT_LEFT
		// 2 = MRK_FOOT_RIGHT
		int start = (int)(pPlayer.Position.Y + pPlayer.Dimensions.Y / 2.0f);
		int finish = (int)(pPlayer.Position.Y + pPlayer.Velocity.Y + pPlayer.RaycastInfos[i].Position.Y);
		int x = (int)(pPlayer.Position.X + pPlayer.RaycastInfos[i].Position.X);
		for (int y = start; y <= finish; ++y)
		{
			if (CheckPoint(pMap, x, y, hitData))
			{
				int id = pMap.Data[hitData.TileX][hitData.TileY].CollisionID;
				if (!((i == 1 || i == 2) && id != 0))
				{
					if (pMap.BitMasks[id][hitData.PixelX][hitData.PixelY])
					{
						pPlayer.Position.Y = (float)y - pPlayer.RaycastInfos[i].Position.Y;
						pPlayer.Velocity.Y = 0.0f;
						hitVert = true;
						pPlayer.IsGrounded = true;
						break;
					}
				}
			}
		}
		if (hitVert)
			break;
	}

	// top collision
	for (int i = 9; i <= 11; ++i)
	{
		// 9 = MRK_HEAD_CENTER
		// 10 = MRK_HEAD_LEFT
		// 11 = MRK_HEAD_RIGHT
		int start = (int)(pPlayer.Position.Y + pPlayer.Dimensions.Y / 2.0f);
		int finish = (int)(pPlayer.Position.Y + pPlayer.Velocity.Y + pPlayer.RaycastInfos[i].Position.Y);
		int x = (int)(pPlayer.Position.X + pPlayer.RaycastInfos[i].Position.X);
		for (int y = start; y >= finish; --y)
		{
			if (CheckPoint(pMap, x, y, hitData))
			{
				pPlayer.Position.Y = (float)(y + 1) + pPlayer.RaycastInfos[i].Position.Y;
				pPlayer.Velocity.Y = 0.0f;
				hitVert = true;
				break;
			}
		}
		if (hitVert)
			break;
	}

	// progress with vertical movement if no collision
	if (!hitVert)
		pPlayer.Position.Y += pPlayer.Velocity.Y;
	*/
}

bool World::CheckPoint(const Map& pMap, int pX, int pY, HitData& pHitData)
{
	/*
	// Get the tile coordinates.
	pHitData.TileX = pX / pMap.Tileset.Size;
	pHitData.TileY = pY / pMap.Tileset.Size;
	// Get the inter-tile pixel coordinates.
	pHitData.PixelX = pX % pMap.Tileset.Size;
	pHitData.PixelY = pY % pMap.Tileset.Size;

	// Check if within map bounds.
	if (pHitData.TileX < 0 || pHitData.TileX >= pMap.Width || pHitData.TileY < 0 || pHitData.TileY >= pMap.Height)
		return false;

	// Grab the tile.
	Tile* tile = &pMap.Data[pHitData.TileX][pHitData.TileY];
	if (tile->CollisionID < 0)
		return false;

	// Check against bitmask.
	return pMap.BitMasks[tile->CollisionID][pHitData.PixelX][pHitData.PixelY];
	*/

	return false;
}