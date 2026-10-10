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
