// Direction.hpp

#ifndef __DIRECTION_HPP__
#define __DIRECTION_HPP__

enum Direction
{
	None = -1,
	Up = 0,
	Down = 1,
	Left = 2,
	Right = 3,
};

Direction OppositeDirection(Direction pDirection);

#endif