#pragma once

#include <string>

#include "json.hpp"
#include "../has_fields.h"


enum AssetType {
	NONE,
	TEXTURE,
	MATERIAL,
	SCRIPT,
	SOUND,
	VERTICES
};


class Asset: public HasFields {
public:
	std::string name;
	AssetType type = AssetType::NONE;

	Asset();
};

void to_json(nlohmann::json& j, const Asset& asset);
void from_json(const nlohmann::json& j, Asset& asset);