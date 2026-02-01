#pragma once

#include "assets/script.h"
#include "assets/material.h"
#include "assets/texture.h"
#include "assets/vertices.h"

class AssetCreator {
public:
	static Script* createScript(std::string name, long long id = -1) {
		logDebug("[ASSET CREATOR] createScript ", name, " ", id);
		Script* script = new Script(id);
		script->name = name;
		return script;
	}

	static Material* createMaterial(std::string name, long long id = -1) {
		logDebug("[ASSET CREATOR] createMaterial ", name, " ", id);
		Material* material = new Material(id);
		material->name = name;
		return material;
	}

	static Texture* createTexture(std::string name, long long id = -1) {
		logDebug("[ASSET CREATOR] createTexture ", name, " ", id);
		Texture* texture = new Texture(id);
		texture->name = name;
		return texture;
	}

	static Vertices* createVertices(std::string name, long long id = -1) {
		logDebug("[ASSET CREATOR] createVertices ", name, " ", id);
		Vertices* vertices = new Vertices(id);
		vertices->name = name;
		return vertices;
	}

	template<typename T>
	static T* createAsset(std::string name, long long id = -1) {
		T* asset = new T(id);
		asset->name = name;
		return asset;
	}
};