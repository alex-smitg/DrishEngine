#pragma once

#include <vector>
#include <string>

#include "miniaudio.h"

#include "asset.h"

class Sound : public Asset {
public:
	std::filesystem::path path;

	ma_sound sound;
	ma_decoder decoder;
	
	Sound() {
		type = AssetType::SOUND;
	}
};

