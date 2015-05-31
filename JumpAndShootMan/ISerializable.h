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
		virtual bool Serialize(Json::Value& p_Root) const = 0;
		bool SerializeToFile(const std::string& p_Filename) const;

		virtual bool Deserialize(const Json::Value& p_Root) = 0;
		bool DeserializeFromFile(const std::string& p_Filename);
};

#endif
