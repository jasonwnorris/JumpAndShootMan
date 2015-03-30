#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__

#include <string>

#include <SDL2\SDL.h>

namespace HGF
{
	class Texture
	{
		public:
			Texture();
			~Texture();

			unsigned int GetWidth() const;
			unsigned int GetHeight() const;
			unsigned int GetBPP() const;
			unsigned int GetID() const;
			Uint32* GetPixels() const;
			SDL_PixelFormat* GetFormat() const;

			bool Load(const std::string& pFilename);
			void GetColor(int pX, int pY, Uint8& pRed, Uint8& pGreen, Uint8& pBlue, Uint8& pAlpha);

		private:
			SDL_Surface* mSurface;
			unsigned int mWidth;
			unsigned int mHeight;
			unsigned int mBPP;
			unsigned int mID;
	};
}

#endif