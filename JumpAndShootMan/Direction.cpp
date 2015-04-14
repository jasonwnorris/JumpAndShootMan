// Direction.cpp

// Project Includes
#include "Direction.hpp"

Direction OppositeDirection(Direction pDirection)
{
	switch (pDirection)
	{
		case Direction::Up:
			return Direction::Down;
		case Direction::Down:
			return Direction::Up;
		case Direction::Left:
			return Direction::Right;
		case Direction::Right:
			return Direction::Left;
		default:
			return Direction::None;
	}
}