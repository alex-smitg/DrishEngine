#include "asset.h"

Asset::Asset(long long id) {
	addField(Field("Name", FieldType::String, 1, &name));

	if (id == -1) {
		assId = nextAssetId;
		nextAssetId++;
	}
	else {
		assId = id;
	}
}


void to_json(nlohmann::json& j, const Asset& asset) {
	j["name"] = asset.name;
	j["assId"] = asset.assId;
}

void from_json(const nlohmann::json& j, Asset& asset) {
	j.at("name").get_to(asset.name);
	j.at("assId").get_to(asset.assId);
}