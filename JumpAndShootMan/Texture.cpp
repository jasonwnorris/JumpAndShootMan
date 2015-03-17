#include "Texture.hpp"

#include <SDL2\SDL_image.h>
#include <SDL2\SDL_opengl.h>

namespace HGF
{
	Texture::Texture()
	{
	}

	Texture::~Texture()
	{
		// free the surface
		SDL_FreeSurface(mSurface);

		// delete the texture
		glDeleteTextures(1, &mID);
	}

	unsigned int Texture::GetWidth() const
	{
		return mWidth;
	}

	unsigned int Texture::GetHeight() const
	{
		return mHeight;
	}

	unsigned int Texture::GetBPP() const
	{
		return mBPP;
	}

	unsigned int Texture::GetID() const
	{
		return mID;
	}

	Uint32* Texture::GetPixels() const
	{
		return (Uint32*)mSurface->pixels;
	}

	SDL_PixelFormat* Texture::GetFormat() const
	{
		return mSurface->format;
	}

	bool Texture::Load(const std::string& pFilename)
	{
		// load the surface
		mSurface = IMG_Load(pFilename.c_str());

		// break out if surface is null
		if (mSurface == NULL)
			return false;

		// generate the texture
		glGenTextures(1, &mID);

		// bind the texture
		glBindTexture(GL_TEXTURE_2D, mID);

		// get the width, height, and bits-per-pixel
		mWidth = mSurface->w;
		mHeight = mSurface->h;
		mBPP = mSurface->format->BitsPerPixel;

		// see if the surface contains and alpha channel
		int mode = GL_RGB;
		if (mBPP == 32)
			mode = GL_RGBA;

		// create a texture from the surface's pixels array
		glTexImage2D(GL_TEXTURE_2D, 0, mode, mWidth, mHeight, 0, mode, GL_UNSIGNED_BYTE, mSurface->pixels);

		// set min and mag filters to linear
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// unbind the texture
		glBindTexture(GL_TEXTURE_2D, 0);

		return true;
	}
}