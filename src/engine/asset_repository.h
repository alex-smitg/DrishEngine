#pragma once

#include <vector>
#include <string>
#include <map>

#include "assets/script.h"
#include "assets/material.h"
#include "assets/texture.h"
#include "assets/vertices.h"

#include "asset_creator.h"

#include "loaders/image_loader.h"
#include "loaders/model_loader.h"
#include "logger.h"

class AssetRepository {
public:
	std::map<long long, Script*> scriptsMap;
	std::map<long long, Material*> materialsMap;
	std::map<long long, Texture*> texturesMap;
	std::map<long long, Vertices*> verticesMap;
	
	Shader defaultShader;
	Material* defaultMaterial;
	
	AssetRepository() {
		defaultMaterial = AssetCreator::createMaterial("default", -2);
		defaultMaterial->shader = &defaultShader;
	}
	

	void addMaterial(Material* material) {
		materialsMap[material->assId] = material;
	}

	void addScript(Script* script) {
		scriptsMap[script->assId] = script;
	}

	void addVertices(Vertices* vertices) {
		verticesMap[vertices->assId] = vertices;
	}

	void addTexture(Texture* texture) {
		texturesMap[texture->assId] = texture;
	}


	Script* getScript(long long id) {
		return scriptsMap[id];
	}

	Vertices* getVertices(long long id) {
		
		return verticesMap[id];
	}

	Texture* getTexture(long long id) {

		return texturesMap[id];
	}

	Material* getMaterial(long long id) {
		if (materialsMap.contains(id)) {
			return materialsMap[id];
		}
		else {
			return nullptr;
		}
	}
};

