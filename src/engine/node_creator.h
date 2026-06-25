#pragma once

#include "game_objects/node.h"
#include "game_objects/model.h"
#include "game_objects/camera.h"
#include "game_objects/point_light.h"
#include "game_objects/sound_player.h"

#include "game_object_types.h"
#include "node_repository.h"

inline long long nextNodeId = 0;


class NodeCreator {
public:
	static Node* createNode(Type type,
			std::string name,
			NodeRepository* nodeRepository,
			long long id = -1) {
		logInfo("Create ", name);


		Node* node = nullptr;

		switch (type)
		{
		case BASE:
			node = new Node();
			break;
		case MODEL:
		{
			Model* model = new Model();
			node = model;
			nodeRepository->add(model);
			break;
		}
		case CAMERA:
		{
			Camera* camera = new Camera();
			node = camera;
			nodeRepository->add(camera);
			break;
		}
		case POINT_LIGHT:
		{
			PointLight* pointLight = new PointLight();
			node = pointLight;
			nodeRepository->add(pointLight);
			break;
		}
		case SOUND_PLAYER:
		{
			SoundPlayer* soundPlayer = new SoundPlayer();
			node = soundPlayer;
			nodeRepository->add(soundPlayer);
			break;
		}

		default:
			node = new Node();
			break;
		}

		if (id == -1) {
			node->id = nextNodeId;
			nextNodeId++;
		}
		else {
			node->id = id;
		}

		node->name = name;

		logDebug(node->name, ": type: ", type, ", ", "id: ", node->id);

		return node;
	}
};