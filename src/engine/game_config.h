#pragma once

#include <string>

struct GameConfig {
	int width = 500;
	int height = 500;
	std::string title = "Game";
	bool useFullscreen = false;
};

inline void to_json(nlohmann::json& j, const GameConfig& config) {
	j["width"] = config.width;
	j["height"] = config.height;
	j["title"] = config.title;
	j["useFullscreen"] = config.useFullscreen;
}