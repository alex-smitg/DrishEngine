#pragma once

#include <string>

#include "asset.h"

class Script : public Asset {
public:
	std::string source;

	using Asset::Asset;
};

inline void to_json(nlohmann::json& j, const Script& script) {
	nlohmann::to_json(j, static_cast<Asset>(script));
	j["source"] = script.source;
}
