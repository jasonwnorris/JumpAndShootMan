#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include "Vector2.hpp"

struct Camera
{
	HGF::Vector2 Position;
	float Rotation;
	float Zoom;
};

#endif