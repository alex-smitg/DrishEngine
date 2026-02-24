#pragma once

#include "gl_common.h"
#include "node_repository.h"
#include "game_objects/camera.h"
#include "shader.h"

class Looper {
public:
	bool debugDraw = false;

	Shader* shader = nullptr;
	Camera* currentCamera = nullptr;

	void loop(NodeRepository* nodeRepository, AssetRepository* assetRepository) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		
		

		for (int i = nodeRepository->cameras.size(); i > 0; i--) {
			Camera* camera = nodeRepository->cameras[i - 1];
			if (currentCamera != nullptr) {
				if (!currentCamera->active) {
					currentCamera = nullptr;
				}
			}
			else {
				if (camera->active) {
					currentCamera = camera;
				}
			}
		}

		

		for (auto const& pair : assetRepository->materialsMap) {
			Material* material = pair.second;
			shader = material->shader;
			material->shader->use();
			if (currentCamera != nullptr) {
				material->shader->setMat4("projection", currentCamera->perspective);
				if (currentCamera->create_view) {
					currentCamera->view = glm::inverse(currentCamera->transform.getMatrix());
				} 
				material->shader->setMat4("view", currentCamera->view);
			}
			
		}
		if (shader != nullptr) {
			int n = 0;
			shader->use();
			if (currentCamera != nullptr) {
				shader->setVec3("viewPos", currentCamera->transform.position);
			}
			
			shader->setInt("pointLightsCount", nodeRepository->pointLights.size());
			for (int i = nodeRepository->pointLights.size(); i > 0; i--) {
				PointLight* pointLight = nodeRepository->pointLights[i - 1];
				shader->setFloat("pointLights[" + std::to_string(n) + "].radius", pointLight->radius);
				shader->setVec3("pointLights[" + std::to_string(n) + "].position", pointLight->transform.position);
				shader->setVec3("pointLights[" + std::to_string(n) + "].color", pointLight->color);
				shader->setFloat("pointLights[" + std::to_string(n) + "].strength", pointLight->strength);
				//shader.setFloat("pointLights[" + std::to_string(n) + "].radius", light->radius);
				n++;
			}
		}

		for (int i = nodeRepository->models.size(); i > 0; i--) {
			Model* model = nodeRepository->models[i - 1];
			if (model->material != nullptr) {
				model->material->shader->setVec3("color", model->material->color);
				if (model->material->texture == nullptr) {
					model->material->shader->setInt("useTexture", 0);
				}
				else {
					model->material->shader->setInt("useTexture", 1);
					glBindTexture(GL_TEXTURE_2D, model->material->texture->glid);
				}
				model->material->shader->setFloat("shine", model->material->shine);
				model->material->shader->setMat4("model", model->transform.getMatrix());
				model->material->shader->setInt("useLight", model->material->useLight);
				if (model->vertices != nullptr) {
					model->vertices->draw();
				}
			}
		}
	}
};