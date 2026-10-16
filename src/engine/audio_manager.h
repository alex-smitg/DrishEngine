#pragma once
#include "logger.h"


#include "miniaudio.h"


class AudioManager {
public:
	ma_engine engine;

	AudioManager() {
		ma_engine_init(NULL, &engine);
	}
};