// TextureManager.hpp

#ifndef __TEXTUREMANAGER_HPP__
#define __TEXTUREMANAGER_HPP__

// HGF Includes
#include <HGF\Texture.hpp>
// STL Includes
#include <map>
#include <string>

class TextureManager
{
	private:
		TextureManager();
		~TextureManager();

	public:
		static HGF::Texture* Load(const std::string& p_Filename, HGF::Interpolation p_Interpolation = HGF::Interpolation::Linear, HGF::Wrapping p_Wrapping = HGF::Wrapping::ClampToEdge);

	private:
		static std::map<std::string, HGF::Texture*> s_TextureMap;
};

#endif
