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


	void add(Camera* camera) {
		cameras.push_back(camera);
	}

	void add(Model* model) {
		models.push_back(model);
	}

	void add(PointLight* pointLight) {
		pointLights.push_back(pointLight);
	}
};