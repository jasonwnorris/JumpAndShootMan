// Globals.hpp

#ifndef __GLOBALS_HPP__
#define __GLOBALS_HPP__

#include <gl\glew.h>
#include <SDL2\SDL.h>
#include <SDL2\SDL_image.h>

#define GLM_FORCE_RADIANS
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\gtc\type_ptr.hpp>

class Globals
{
	public:
		static bool IsDebugDrawOn;
		static int ScreenWidth;
		static int ScreenHeight;
};

#endif