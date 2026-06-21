#pragma once

#include <vector>

#include "game_objects/point_light.h"
#include "game_objects/model.h"
#include "game_objects/camera.h"

class NodeRepository {
public:
	std::vector<PointLight*> pointLights;
	std::vector<Model*> models;
	std::vector<Camera*> cameras;

	std::vector<Node*> allObjects;


	void deleteNode(Node* node) {
		switch (node->type)
		{
		case Type::CAMERA:
			cameras.erase(std::remove(cameras.begin(), cameras.end(), static_cast<Camera*>(node)), cameras.end());
		break;
		case Type::MODEL:
			models.erase(std::remove(models.begin(), models.end(), static_cast<Model*>(node)), models.end());
			break;
		case Type::POINT_LIGHT:
			pointLights.erase(std::remove(pointLights.begin(), pointLights.end(), static_cast<PointLight*>(node)), pointLights.end());
			break;

		default:
			break;
		}
		allObjects.erase(std::remove(allObjects.begin(), allObjects.end(), node), allObjects.end());
	}

	void add(Camera* camera) {
		cameras.push_back(camera);
		allObjects.push_back(camera);
	}

	void add(Model* model) {
		models.push_back(model);
		allObjects.push_back(model);
	}

	void add(PointLight* pointLight) {
		pointLights.push_back(pointLight);
		allObjects.push_back(pointLight);
	}
};