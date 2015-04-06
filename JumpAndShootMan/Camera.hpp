// Camera.hpp

#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

// HGF Includes
#include <HGF\Vector2.hpp>

struct Camera
{
	HGF::Vector2 Position;
	float Rotation;
	float Zoom;
};

#endif