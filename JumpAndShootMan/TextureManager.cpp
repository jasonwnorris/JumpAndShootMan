// TextureManager.cpp

// Project Includes
#include "TextureManager.hpp"

std::map<std::string, HGF::Texture*> TextureManager::s_TextureMap;

TextureManager::TextureManager()
{
}

TextureManager::~TextureManager()
{
}

HGF::Texture* TextureManager::Load(const std::string& p_Filename, HGF::Interpolation p_Interpolation, HGF::Wrapping p_Wrapping)
{
	std::map<std::string, HGF::Texture*>::iterator iter = s_TextureMap.find(p_Filename);
	if (iter == s_TextureMap.end())
	{
		s_TextureMap[p_Filename] = new HGF::Texture();
		s_TextureMap[p_Filename]->Load(p_Filename, p_Interpolation, p_Wrapping);
	}

	return s_TextureMap[p_Filename];
}
