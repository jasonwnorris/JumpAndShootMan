// main.cpp

// Project Includes
#include "JumpShootGame.hpp"
// STL Includes
#include <time.h>

int main(int argc, char** argv)
{
	srand((unsigned int)time(NULL));

	JumpShootGame game;
	game.Run();

	return 0;
}