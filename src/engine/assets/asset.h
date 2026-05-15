#pragma once

#include <string>

#include "json.hpp"

#include "../has_fields.h"

inline int nextAssetId = 0;

const int AUTO_INCREMENT_ID = -1;

typedef long AssetID;

enum AssetType {
	TEXTURE,
	MATERIAL,
	SCRIPT,
	SOUND,
	VERTICES
};


class Asset: public HasFields {
public:
	std::string name;
	AssetID assId;

	Asset(long long id);

};

void to_json(nlohmann::json& j, const Asset& asset);
void from_json(const nlohmann::json& j, Asset& asset);