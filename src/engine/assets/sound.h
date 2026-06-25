#pragma once

#include <vector>
#include <string>

#include "miniaudio.h"

#include "asset.h"

class Sound : public Asset {
public:
	std::filesystem::path path;

	static ma_engine engine;

	/*ma_result result;
ma_engine engine;

result = ma_engine_init(NULL, &engine);
if (result != MA_SUCCESS) {
	return -1;
}

ma_sound sound;

result = ma_sound_init_from_file(&engine, "sound.wav", 0, NULL, NULL, &sound);
if (result != MA_SUCCESS) {
	std::cout << ma_result_description(result);
	return -1;
}



ma_sound_set_looping(&sound, true);
ma_sound_start(&sound)*/
	;
	// ma_sound_init_from_data_source()

	Sound() {
		type = AssetType::SOUND;
	}
};