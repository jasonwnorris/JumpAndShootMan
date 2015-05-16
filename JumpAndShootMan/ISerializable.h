// ISerializable.hpp

#ifndef __SERIALIZABLE_HPP__
#define __SERIALIZABLE_HPP__

// JsonCpp Includes
#include <json\json.h>
// STL Includes
#include <string>

class ISerializable
{
	public:
		virtual bool Serialize(Json::Value& pRoot) const = 0;
		bool SerializeToFile(const std::string& pFilename) const;

		virtual bool Deserialize(const Json::Value& pRoot) = 0;
		bool DeserializeFromFile(const std::string& pFilename);
};

#endif