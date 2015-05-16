// ISerializable.cpp

// Project Includes
#include "ISerializable.h"
// STL Includes
#include <fstream>

bool ISerializable::SerializeToFile(const std::string& pFilename) const
{
	Json::Value root;
	if (!Serialize(root))
		return false;

	std::ofstream file;
	file.open(pFilename, std::ofstream::out | std::ofstream::trunc);
	if (!file.is_open())
		return false;

	Json::FastWriter writer;
	file << writer.write(root);
	file.close();
	
	return true;
}

bool ISerializable::DeserializeFromFile(const std::string& pFilename)
{
	std::ifstream file;
	file.open(pFilename, std::ifstream::in);
	if (!file.is_open())
		return false;

	Json::Value root;
	Json::Reader reader;
	if (!reader.parse(file, root, false))
		return false;

	return Deserialize(root);
}