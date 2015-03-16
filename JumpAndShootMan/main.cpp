#include <SDL2\SDL.h>
#include <SDL2\SDL_image.h>

#include "MathHelper.hpp"
#include "Game.hpp"
#include "Vector2.hpp"

#include <cstdlib>
#include <time.h>

int main(int argc, char** argv)
{
	srand((unsigned int)time(NULL));

	Game game;
	game.Run();

	return 0;
}