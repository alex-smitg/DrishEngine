#pragma once

#include "node.h"

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

inline void to_json(nlohmann::json& j, const SoundPlayer& soundPlayer)
{
	nlohmann::to_json(j, static_cast<Node>(soundPlayer));
	j["volume"] = soundPlayer.volume;
	j["loop"] = soundPlayer.loop;
}