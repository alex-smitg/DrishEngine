#pragma once

#include "node.h"
#include "../transform.h"

class SoundPlayer : public Node {
public:
	AssetHandle soundHandle;

	float volume = 1.0;
	bool loop = false;

	SoundPlayer() {
		this->type = Type::SOUND_PLAYER;

		addField(Field("Sound", FieldType::SoundHandle, &soundHandle));
		addField(Field("Volume", FieldType::Float, &volume));
		addField(Field("Loop", FieldType::Boolean, &loop));
	}
};