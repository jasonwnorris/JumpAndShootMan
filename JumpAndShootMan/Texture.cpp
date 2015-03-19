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
		// Free the surface.
		SDL_FreeSurface(mSurface);

		// Delete the texture.
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
		// Load the surface.
		mSurface = IMG_Load(pFilename.c_str());

		// Break out if surface is NULL.
		if (mSurface == NULL)
			return false;

		// Generate the texture.
		glGenTextures(1, &mID);

		// Bind the texture.
		glBindTexture(GL_TEXTURE_2D, mID);

		// Get the width, height, and bits-per-pixel.
		mWidth = mSurface->w;
		mHeight = mSurface->h;
		mBPP = mSurface->format->BitsPerPixel;

		// See if the surface contains an alpha channel.
		int mode = GL_RGB;
		if (mBPP == 32)
			mode = GL_RGBA;

		// Create a texture from the surface's pixels array.
		glTexImage2D(GL_TEXTURE_2D, 0, mode, mWidth, mHeight, 0, mode, GL_UNSIGNED_BYTE, mSurface->pixels);

		// Set min and mag filters to nearest.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// Unbind the texture.
		glBindTexture(GL_TEXTURE_2D, 0);

		return true;
	}

	void Texture::GetColor(int pX, int pY, Uint8& pRed, Uint8& pGreen, Uint8& pBlue, Uint8& pAlpha)
	{
		Uint8* pixel = (Uint8*)mSurface->pixels + pY * mSurface->pitch + pX * mSurface->format->BytesPerPixel;
		Uint32 color = 0;

		switch (mSurface->format->BytesPerPixel)
		{
			case 1:
				color = *pixel;
				break;
			case 2:
				color = *(Uint16*)pixel;
				break;
			case 3:
				if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
					color = pixel[0] << 16 | pixel[1] << 8 | pixel[2];
				else
					color = pixel[0] | pixel[1] << 8 | pixel[2] << 16;
				break;
			case 4:
				color = *(Uint32*)pixel;
				break;
			default:
				color = 0;
				break;
		}

		SDL_GetRGBA(color, mSurface->format, &pRed, &pGreen, &pBlue, &pAlpha);
	}
}