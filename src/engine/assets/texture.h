#pragma once

#include <string>
#include <filesystem>

#include "asset.h"

#include "../field.h"

class Texture : public Asset {
public:
	int width = 0;
	int height = 0;
	int channels = 0;
	unsigned int glid;

	std::filesystem::path path;

	Texture() {
		type = AssetType::TEXTURE;
		addField(Field("Image", FieldType::TextureID, &glid));
	}
};

inline void to_json(nlohmann::json& j, const Texture& texture) {
	nlohmann::to_json(j, static_cast<Asset>(texture));
	j["path"] = texture.path;
}
