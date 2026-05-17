#include "asset.h"

Asset::Asset() {
	addField(Field("Name", FieldType::String, &name));
}


void to_json(nlohmann::json& j, const Asset& asset) {
	j["name"] = asset.name;
}

void from_json(const nlohmann::json& j, Asset& asset) {
	j.at("name").get_to(asset.name);
}